/* ------------------------------------------------------------------   */
/*      item            : TimingViewGtk.cxx
        made by         : Rene' van Paassen
        date            : 020225
        category        : body file
        description     :
        changes         : 020225 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#include "DataTimeSpec.hxx"
#include "SyncReport.hxx"
#include "TimingResults.hxx"
#include "gio/gio.h"
#include <cstddef>
#define TimingViewGtk_cxx

#include <dueca-conf.h>

// this module is conditional on having and using libglade

#include "TimingViewGtk.hxx"
#include "GtkDuecaView.hxx"
#include "DuecaPath.hxx"
#include <boost/format.hpp>
#include <time.h>
#include <WrapSendEvent.hxx>

// #define I_SYS
#define W_CNF
#define E_CNF
#include "debug.h"

#define DO_INSTANTIATE
#define NO_TYPE_CREATION
#include "dueca.h"
#include <WrapSendEvent.hxx>

// only useful if interfaces present

DUECA_NS_START

/** Toolkit-dependent GUI information. */
class TimingViewGtk::GuiInfo
{
  friend class TimingViewGtk;

  /** File for the interface definition. */
  std::string gladefile;

  /** Object that reads the glade file, and builds up the window. */
  GtkGladeWindow gwindow;

  /** Table with timing results. */
  GtkWidget *table;

  /** List store for timing results table */
  GListStore *table_store;

  /** The item we add to the menu of DUECA's main window. */
  GMenuItem *menuitem;

  /** List store for sync status table */
  GListStore *synctable_store;

  /** Constructor. */
  GuiInfo(const vstring &gladefile) :
    gladefile(gladefile),
    table(NULL),
    menuitem(NULL)
  {}
};

struct _DTimingInfo
{
  GObject parent;
  unsigned node;
  SyncReport sync;
};

G_DECLARE_FINAL_TYPE(DTimingInfo, d_timing_info, D, TIMING_INFO, GObject);
G_DEFINE_TYPE(DTimingInfo, d_timing_info, G_TYPE_OBJECT);
static void d_timing_info_class_init(DTimingInfoClass *klass) {}
static void d_timing_info_init(DTimingInfo *self) {}

static DTimingInfo *d_timing_info_new(unsigned node, const SyncReport &data)
{
  auto res = D_TIMING_INFO(g_object_new(d_timing_info_get_type(), NULL));
  res->node = node;
  res->sync = data;
  return res;
}

struct _DTimingSummary
{
  GObject parent;
  std::string maker_and_act;
  TimeTickType tstart;
  TimingResults data;
};

G_DECLARE_FINAL_TYPE(DTimingSummary, d_timing_summary, D, TIMING_SUMMARY,
                     GObject);
G_DEFINE_TYPE(DTimingSummary, d_timing_summary, G_TYPE_OBJECT);
static void d_timing_summary_class_init(DTimingSummaryClass *klass) {}
static void d_timing_summary_init(DTimingSummary *self) {}

static DTimingSummary *d_timing_summary_new(const std::string &maker_and_act,
                                            TimeTickType tstart,
                                            const TimingResults &data)
{
  auto res = D_TIMING_SUMMARY(g_object_new(d_timing_summary_get_type(), NULL));
  res->maker_and_act = maker_and_act;
  res->tstart = tstart;
  res->data = data;
  return res;
}

TimingViewGtk::TimingViewGtk(Entity *e, const char *part,
                             const PrioritySpec &ps) :
  TimingView(e, part, ps),
  gui(*(new GuiInfo(DuecaPath::prepend("timingview-gtk4.ui"))))
{
  // check the presence of a DuecaView object, for getting initial
  // access to the interface
  if (GtkDuecaView::single() == NULL) {
    /* DUECA UI.

       To use timingview, DuecaView needs to be configured first. */
    W_CNF(getId() << " TimingView needs DuecaView!");
    can_start = false;
    return;
  }
}

// table with callback functions for glade xml window.
static GladeCallbackTable cb_links[] = {
  { "timingview_clear", "clicked", gtk_callback(&TimingViewGtk::clearView) },
  { "update_sync", "clicked", gtk_callback(&TimingViewGtk::requestSync),
    reinterpret_cast<gpointer>(0) },
  { "clear_sync", "clicked", gtk_callback(&TimingViewGtk::requestSync),
    reinterpret_cast<gpointer>(1) },
  { "timingview_close", "clicked",
    gtk_callback(&TimingViewGtk::activateMenuItem) },
  { "timingview_window", "close-request",
    gtk_callback(&TimingViewGtk::deleteView) },
  { NULL, NULL, NULL }
};

bool TimingViewGtk::complete()
{
  gui.gwindow.readGladeFile(gui.gladefile.c_str(), "timingview_window",
                            reinterpret_cast<gpointer>(this), cb_links);
  GtkWidget *window = gui.gwindow["timingview_window"];

  // attach a store
  gui.table = gui.gwindow["timingtable"];
  gui.table_store = g_list_store_new(d_timing_summary_get_type());
  auto selection = gtk_single_selection_new(G_LIST_MODEL(gui.table_store));
  gtk_column_view_set_model(GTK_COLUMN_VIEW(gui.table),
                            GTK_SELECTION_MODEL(selection));
  g_object_unref(selection);

  // also to the table with sync summary
  auto synctable = gui.gwindow["timingtable"];
  gui.synctable_store = g_list_store_new(d_timing_info_get_type());
  auto selection2 = gtk_single_selection_new(G_LIST_MODEL(gui.synctable_store));
  // fill that table with zeros
  for (int n = 0; n < no_nodes; n++) {
    g_list_store_append(gui.synctable_store,
                        d_timing_info_new(n, SyncReport()));
  }
  gtk_column_view_set_model(GTK_COLUMN_VIEW(synctable),
                            GTK_SELECTION_MODEL(selection2));
  g_object_unref(selection2);

  // request the DuecaView object to make an entry for my window,
  // opening it on activation
  gui.menuitem =
    GtkDuecaView::single()->requestViewEntry("Timing View", GTK_WIDGET(window));

  return can_start;
}

TimingViewGtk::~TimingViewGtk()
{
  g_object_unref(gui.table_store);
  g_object_unref(gui.synctable_store);
}

void TimingViewGtk::appendReport(const std::string &maker_and_act,
                                 const TimeTickType &tstart,
                                 const TimingResults &data)
{
  auto report = d_timing_summary_new(maker_and_act, tstart, data);
  g_list_store_append(gui.table_store, report);
  if (num_rows++ > 200) {
    g_list_store_remove(gui.table_store, 0);
  }
  g_object_unref(report);
}

void TimingViewGtk::clearView(GtkButton *button, gpointer user_data)
{
  g_list_store_remove_all(gui.table_store);
}

void TimingViewGtk::updateSync(int node, const SyncReport &report)
{
  auto sync = d_timing_info_new(node, report);
  g_list_store_splice(gui.synctable_store, node, 1,
                      reinterpret_cast<gpointer *>(&sync), 1);
  g_object_unref(sync);
}

void TimingViewGtk::activateMenuItem(GtkButton *button, gpointer user_data)
{
  g_signal_emit_by_name(G_OBJECT(gui.menuitem), "activate", NULL);
}

gboolean TimingViewGtk::deleteView(GtkWidget *window, GdkEvent *event,
                                   gpointer user_data)
{
  g_signal_emit_by_name(G_OBJECT(gui.menuitem), "activate", NULL);

  // with this, the click is handled.
  return TRUE;
}

void TimingViewGtk::requestSync(GtkButton *button, gpointer user_data)
{
  bool clearflag = user_data != NULL;
  /* DUECA UI.

     Information on the callback for requesting sync. */
  I_SYS(getId() << " request sync with " << clearflag);
  if (!request_report.isValid()) {
    /* DUECA UI.

       The channel for requesting timing information is not valid. */
    W_SYS(getId() << " channel for requests not valid");
    return;
  }
  wrapSendEvent(request_report, new SyncReportRequest(clearflag),
                SimTime::now());
}

void cbSetupLabel(GtkSignalListItemFactory *fact, GtkListItem *item,
                  gpointer user_data)
{
  auto label = gtk_label_new("");
  gtk_list_item_set_child(item, label);
  g_object_unref(label);
}

void cbBindTimingNode(GtkSignalListItemFactory *fact, GtkListItem *item,
                      gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label,
                     boost::str(boost::format("%2d") % row->node).c_str());
}

void cbBindTimingDiff(GtkSignalListItemFactory *fact, GtkListItem *item,
                      gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label, boost::str(boost::format("%5d") % row->sync.difference).c_str());
}

void cbBindTimingNEarly(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label, boost::str(boost::format("%5d") % row->sync.no_early).c_str());
}

void cbBindTimingNLate(GtkSignalListItemFactory *fact, GtkListItem *item,
                       gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label, boost::str(boost::format("%5d") % row->sync.no_late).c_str());
}

void cbBindTimingNDouble(GtkSignalListItemFactory *fact, GtkListItem *item,
                         gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label,
    boost::str(boost::format("%5d") % row->sync.no_double_waits).c_str());
}

void cbBindTimingNNoWait(GtkSignalListItemFactory *fact, GtkListItem *item,
                         gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label,
    boost::str(boost::format("%5d") % row->sync.no_cancelled_waits).c_str());
}

void cbBindTimingLatest(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label,
    boost::str(boost::format("%5d") % row->sync.latest_wrt_ideal).c_str());
}

void cbBindTimingEarliest(GtkSignalListItemFactory *fact, GtkListItem *item,
                          gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label,
    boost::str(boost::format("%5d") % row->sync.earliest_wrt_ideal).c_str());
}

void cbBindTimingStepsz(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_INFO(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(
    label,
    boost::str(boost::format("%5d") % row->sync.average_step_size).c_str());
}

void cbBindSummaryActivity(GtkSignalListItemFactory *fact, GtkListItem *item,
                           gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, row->maker_and_act.c_str());
}

void cbBindSummaryLogtime(GtkSignalListItemFactory *fact, GtkListItem *item,
                          gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%8d") % row->tstart).c_str());
}

void cbBindSummaryMinStart(GtkSignalListItemFactory *fact, GtkListItem *item,
                           gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.min_start).c_str());
}

void cbBindSummaryAvgStart(GtkSignalListItemFactory *fact, GtkListItem *item,
                           gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.avg_start).c_str());
}

void cbBindSummaryMaxStart(GtkSignalListItemFactory *fact, GtkListItem *item,
                           gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.max_start).c_str());
}

void cbBindSummaryMinComplete(GtkSignalListItemFactory *fact, GtkListItem *item,
                              gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.min_complete).c_str());
}

void cbBindSummaryAvgComplete(GtkSignalListItemFactory *fact, GtkListItem *item,
                              gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.avg_complete).c_str());
}

void cbBindSummaryMaxComplete(GtkSignalListItemFactory *fact, GtkListItem *item,
                              gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.max_complete).c_str());
}

void cbBindSummaryNWarn(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.n_warning).c_str());
}

void cbBindSummaryNCrit(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.n_critical).c_str());
}

void cbBindSummaryNuser(GtkSignalListItemFactory *fact, GtkListItem *item,
                        gpointer user_data)
{
  auto row = D_TIMING_SUMMARY(gtk_list_item_get_item(item));
  auto label = GTK_LABEL(gtk_list_item_get_child(item));
  gtk_label_set_text(label, boost::str(boost::format("%5d") % row->data.n_user).c_str());
}

DUECA_NS_END
