/* ------------------------------------------------------------------   */
/*      item            : ChannelOverviewGtk4.cxx
        made by         : repa
        from template   : DuecaModuleTemplate.cxx
        template made by: Rene van Paassen
        date            : Mon May  7 15:45:06 2018
        category        : body file
        description     :
        changes         : Mon May  7 15:45:06 2018 first version
        template changes: 030401 RvP Added template creation comment
                          060512 RvP Modified token checking code
                          160511 RvP Some comments updated
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#include "ChannelOverview.hxx"
#include "ChannelReadInfo.hxx"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtk.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#define ChannelOverviewGtk4_cxx

// include the definition of the module class
#include "ChannelOverviewGtk4.hxx"

// include the debug writing header, by default, write warning and
// error messages
#include <debug.h>

// include additional files needed for your calculation here
#include "ChannelDataMonitorGtk4.hxx"
#include "GtkDuecaView.hxx"

#include <boost/lexical_cast.hpp>
#include <dueca/ObjectManager.hxx>
#define DEBPRINTLEVEL -1
#include <debprint.h>
#include <dueca/DuecaPath.hxx>

// the standard package for DUECA, including template source
#define DO_INSTANTIATE
#define NO_TYPE_CREATION
#include <dueca/dueca.h>

DUECA_NS_START

// class/module name
const char *const ChannelOverviewGtk4::classname = "channel-view";

// Parameters to be inserted
const ParameterTable *ChannelOverviewGtk4::getMyParameterTable()
{
  static const ParameterTable parameter_table[] = {

    { "count-delay",
      new VarProbe<_ThisModule_, unsigned>(&_ThisModule_::delay_countcollect),
      "Delay to wait before collecting a requested count" },

    { "glade-file",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::gladefile),
      "Interface description (glade, gtkbuilder) for the channel view window" },

    { "glade-file-monitor",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::monitor_gladefile),
      "Interface description (glade, gtkbuilder) for the monitor windows" },

    /* You can extend this table with labels and MemberCall or
       VarProbe pointers to perform calls or insert values into your
       class objects. Please also add a description (c-style string).

       Note that for efficiency, set_timing_description and
       check_timing_description are pointers to pre-defined strings,
       you can simply enter the descriptive strings in the table. */

    /* The table is closed off with NULL pointers for the variable
       name and MemberCall/VarProbe object. The description is used to
       give an overall description of the module. */
    { NULL, NULL, "A module that presents an overview of channel access" }
  };

  return parameter_table;
}

// data type to hold reference to either channel info,
// entry info or reader info
struct _DChannelInfo
{
  GObject parent;
  std::shared_ptr<ChannelOverview::ChannelInfoSet> channel;
  std::shared_ptr<ChannelOverview::ChannelInfoSet::EntryInfoSet> entry;
  std::shared_ptr<ChannelOverview::ChannelInfoSet::EntryInfoSet::ReadInfoSet>
    reader;
};

// GTK4
// create a type for the DCO data pieces
G_DECLARE_FINAL_TYPE(DChannelInfo, d_channel_info, D, CHANNEL_INFO, GObject);
G_DEFINE_TYPE(DChannelInfo, d_channel_info, G_TYPE_OBJECT);


// constructor
ChannelOverviewGtk4::ChannelOverviewGtk4(Entity *e, const char *part,
                                         const PrioritySpec &ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments. */
  ChannelOverview(e, part, ps),

  // initialize the data you need in your simulation or process
  gladefile(DuecaPath::prepend("channel_overview-gtk4.ui")),
  monitor_gladefile(DuecaPath::prepend("channel_datamonitor-gtk4.ui")),
  window(),
  menuitem(NULL)
{
  //
}

DChannelInfo *
d_channel_info_new(std::shared_ptr<ChannelOverview::ChannelInfoSet> channel)
{
  auto res = D_CHANNEL_INFO(g_object_new(d_channel_info_get_type(), NULL));
  res->channel = channel;
  return res;
}

DChannelInfo *d_channel_info_new(
  std::shared_ptr<ChannelOverview::ChannelInfoSet::EntryInfoSet> entry)
{
  auto res = D_CHANNEL_INFO(g_object_new(d_channel_info_get_type(), NULL));
  res->entry = entry;
  return res;
}

DChannelInfo *d_channel_info_new(
  std::shared_ptr<ChannelOverview::ChannelInfoSet::EntryInfoSet::ReadInfoSet>
    reader)
{
  auto res = D_CHANNEL_INFO(g_object_new(d_channel_info_get_type(), NULL));
  res->reader = reader;
  return res;
}

static void d_channel_info_class_init(DChannelInfoClass *klass)
{
  //
}

static void d_channel_info_init(DChannelInfo *self)
{
  //
}

static GListModel *add_data_element(gpointer _item, gpointer user_data)
{
  auto item = D_CHANNEL_INFO(_item);
  // if this row has a channel, and it has entries, return these in a list
  if (item->channel && item->channel->entries.size()) {
    auto lm = g_list_store_new(d_channel_info_get_type());
    for (auto &e : item->channel->entries) {

      g_list_store_append(lm, gpointer(d_channel_info_new(e)));
    }
    return G_LIST_MODEL(lm);
  }

  // if this row has an entry, and that has readers, return in a list
  if (item->entry && item->entry->rdata.size()) {
    auto lm = g_list_store_new(d_channel_info_get_type());
    for (auto &r : item->entry->rdata) {
      g_list_store_append(lm, gpointer(d_channel_info_new(r)));
    }
    return G_LIST_MODEL(lm);
  }

  // nothing to expand, null list.
  return G_LIST_MODEL(NULL);
}

static GdkPaintable *stream_icon = NULL;
static GdkPaintable *event_icon = NULL;
static GdkPaintable *sequent_icon[2] = { NULL, NULL };
static GdkPaintable *select_icon[3] = { NULL, NULL, NULL };

// namespace

inline GdkPaintable *loadTextureFromFile(const char *fname)
{
  GError *error = NULL;
  auto tex = gdk_texture_new_from_file(
    g_file_new_for_path(DuecaPath::prepend(fname).c_str()), &error);
  if (error) {
    /* DUECA UI.

       Cannot load icon texture. Check DUECA installation.
    */
    E_XTR("Could not load texture \"" << fname << "\": " << error->message);
    g_error_free(error);
    error = NULL;
    return NULL;
  }
  g_object_ref(G_OBJECT(tex));
  return GDK_PAINTABLE(tex);
}

bool ChannelOverviewGtk4::complete()
{
  static GladeCallbackTable cb_table[] = {

    // set of callbacks for the different buttons in the inteface
    { "close", "clicked", gtk_callback(&_ThisModule_::cbClose) },
    { "refresh_times", "clicked",
      gtk_callback(&_ThisModule_::cbRefreshCounts) },
    { "channel_use_view", "delete_event",
      gtk_callback(&_ThisModule_::cbDelete) },

    // for each of the columns in the column view, bind to one of my set-up
    // functions, which create the widgets to be shown there, and to
    // a bind function, which fills these widgets with data from the
    // application model.
    { "fact_channelnum", "setup", gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "fact_channelnum", "bind",
      gtk_callback(&_ThisModule_::cbBindChannelNum) },
    { "fact_channelname", "setup", gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "fact_channelname", "bind",
      gtk_callback(&_ThisModule_::cbBindChannelName) },
    { "fact_entrynum", "setup", gtk_callback(&_ThisModule_::cbSetupExpander) },
    { "fact_entrynum", "bind", gtk_callback(&_ThisModule_::cbBindEntryNum) },
    { "fact_writerid", "setup", gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "fact_writerid", "bind", gtk_callback(&_ThisModule_::cbBindWriterId) },
    { "fact_es", "setup", gtk_callback(&_ThisModule_::cbSetupImage) },
    { "fact_es", "bind", gtk_callback(&_ThisModule_::cbBindES) },
    { "fact_nwrites", "setup", gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "fact_nwrites", "bind", gtk_callback(&_ThisModule_::cbBindNWrites) },
    { "fact_readerid", "setup", gtk_callback(&_ThisModule_::cbSetupExpander) },
    { "fact_readerid", "bind", gtk_callback(&_ThisModule_::cbBindReaderId) },
    { "fact_nreads", "setup", gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "fact_nreads", "bind", gtk_callback(&_ThisModule_::cbBindNReads) },
    { "fact_sel", "setup", gtk_callback(&_ThisModule_::cbSetupImage) },
    { "fact_sel", "bind", gtk_callback(&_ThisModule_::cbBindSel) },
    { "fact_seq", "setup", gtk_callback(&_ThisModule_::cbSetupImage) },
    { "fact_seq", "bind", gtk_callback(&_ThisModule_::cbBindSeq) },
    { "fact_view", "setup", gtk_callback(&_ThisModule_::cbSetupCheckbox) },
    { "fact_view", "bind", gtk_callback(&_ThisModule_::cbBindView) },

    { NULL }
  };

  bool res = ChannelOverview::complete();
  if (!res) {
    /* DUECA UI.

       Error in initialization of ChannelOverview base module.
    */
    E_CNF("failed to start base overview complete");
    return res;
  }

  res = window.readGladeFile(gladefile.c_str(), "channel_use_view",
                             reinterpret_cast<gpointer>(this), cb_table);
  if (!res) {
    /* DUECA UI.

       Cannot find the glade file defining the channel view GUI. Check
       DUECA installation and paths.
    */
    E_CNF("failed to open channel overview " << gladefile);
    return res;
  }

  // access the columnview from the ui file, and link with the data model
  auto channeltree = GTK_COLUMN_VIEW(window["channel_view"]);
  // the backing is a gtk list store with a custom-defined g object, which
  // only links to the data in the application
  auto store = g_list_store_new(d_channel_info_get_type());
  auto model = gtk_tree_list_model_new(G_LIST_MODEL(store), FALSE, FALSE,
                                       add_data_element, NULL, NULL);
  auto selection = gtk_single_selection_new(G_LIST_MODEL(model));
  gtk_column_view_set_model(channeltree, GTK_SELECTION_MODEL(selection));

  // create icons for different channel types
  if (stream_icon == NULL) {
    event_icon = loadTextureFromFile("pixmaps/event-logo.png");
    stream_icon = loadTextureFromFile("pixmaps/stream-logo.png");
    select_icon[0] = loadTextureFromFile("pixmaps/number-logo.png");
    select_icon[1] = loadTextureFromFile("pixmaps/label-logo.png");
    select_icon[2] = loadTextureFromFile("pixmaps/multi-logo.png");
    sequent_icon[0] = loadTextureFromFile("pixmaps/sequential-logo.png");
    sequent_icon[1] = loadTextureFromFile("pixmaps/picking-logo.png");
    if (!event_icon || !stream_icon || !select_icon[0] || !select_icon[1] ||
        !select_icon[2] || !sequent_icon[0] || !sequent_icon[1])
      return false;
  }

  menuitem = GTK_WIDGET(GtkDuecaView::single()->requestViewEntry(
    "Channel View", G_OBJECT(window["channel_use_view"])));

  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */
  return true;
}

// destructor
ChannelOverviewGtk4::~ChannelOverviewGtk4()
{
  //
}

void ChannelOverviewGtk4::reflectChanges(unsigned ichan) 
{
  showChanges();
}

void ChannelOverviewGtk4::reflectChanges(unsigned ichan, unsigned ientry) 
{
  showChanges();
}


void ChannelOverviewGtk4::reflectChanges(unsigned ichan, unsigned ientry,
                                         uint32_t ireader)
{
  // for now, redraw queue the whole?
  showChanges();
}

void ChannelOverviewGtk4::reflectCounts() {
  gtk_widget_queue_draw(GTK_WIDGET(window["col_nwrites"]));
  gtk_widget_queue_draw(GTK_WIDGET(window["col_nreads"]));
}

void ChannelOverviewGtk4::showChanges()
{
  gtk_widget_queue_draw(GTK_WIDGET(window["channel_overview"]));
}

void ChannelOverviewGtk4::cbClose(GtkButton *button, gpointer gp)
{
  g_signal_emit_by_name(G_OBJECT(menuitem), "activate", NULL);
}

void ChannelOverviewGtk4::cbRefreshCounts(GtkButton *button, gpointer gp)
{
  refreshCounts();
}

gboolean ChannelOverviewGtk4::cbDelete(GtkWidget *window, GdkEvent *event,
                                       gpointer user_data)
{
  g_signal_emit_by_name(G_OBJECT(menuitem), "activate", NULL);
  return TRUE;
}

void ChannelOverviewGtk4::closeMonitor(unsigned ichan, unsigned ientry)
{
  infolist[ichan]->entries[ientry]->monitor->close();

  showChanges();
}

void ChannelOverviewGtk4::monitorToggle(GtkToggleButton *btn, _DChannelInfo *ci)
{
  gboolean open = gtk_toggle_button_get_active(btn);

  if (open == TRUE) {
    if (!ci->entry->monitor) {
      ci->entry->monitor = new ChannelDataMonitorGtk4(
        this, ci->entry->wdata.channelid.getObjectId(),
        ci->entry->wdata.entryid, monitor_gladefile);
    }
    ci->entry->monitor->open();
  }
  else {
    ci->entry->monitor->close();
  }

  gtk_widget_queue_draw(GTK_WIDGET(btn));
  //g_signal_emit_by_name(btn, "notify");
  // showChanges();
}

void ChannelOverviewGtk4::cbSetupLabel(GtkSignalListItemFactory *fact,
                                       GtkListItem *item, gpointer user_data)
{
  auto label = gtk_label_new("");
  gtk_list_item_set_child(item, label);
}

void ChannelOverviewGtk4::cbSetupExpander(GtkSignalListItemFactory *fact,
                                          GtkListItem *item, gpointer user_data)
{
  auto label = gtk_label_new("");
  auto expander = gtk_tree_expander_new();
  gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
  gtk_list_item_set_child(item, expander);
}

void ChannelOverviewGtk4::cbSetupImage(GtkSignalListItemFactory *fact,
                                       GtkListItem *item, gpointer user_data)
{
  auto image = gtk_image_new();
  gtk_list_item_set_child(item, image);
}

void ChannelOverviewGtk4_monitorToggle(GObject *w, gpointer self)
{
  DChannelInfo *ci;
  g_object_get(w, "entry_row", &ci, NULL);
  if (self && ci) {
    reinterpret_cast<ChannelOverviewGtk4 *>(self)->monitorToggle(
      GTK_TOGGLE_BUTTON(w), ci);
  }
}

void ChannelOverviewGtk4::cbSetupCheckbox(GtkSignalListItemFactory *fact,
                                          GtkListItem *item, gpointer user_data)
{
  auto checkbox = gtk_toggle_button_new();
  g_signal_connect(G_OBJECT(checkbox), "toggled",
                   G_CALLBACK(ChannelOverviewGtk4_monitorToggle),
                   gpointer(this));
  gtk_list_item_set_child(item, checkbox);
}

void ChannelOverviewGtk4::cbBindChannelNum(GtkSignalListItemFactory *fact,
                                           GtkListItem *item,
                                           gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->channel) {
    auto label = GTK_LABEL(gtk_list_item_get_child(item));
    gtk_label_set_label(
      label, boost::str(boost::format("%d") % row->channel->chanid).c_str());
  }
}

void ChannelOverviewGtk4::cbBindChannelName(GtkSignalListItemFactory *fact,
                                            GtkListItem *item,
                                            gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->channel) {
    auto label = GTK_LABEL(gtk_list_item_get_child(item));
    gtk_label_set_label(label, row->channel->name.c_str());
  }
}

void ChannelOverviewGtk4::cbBindEntryNum(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  auto expander = GTK_TREE_EXPANDER(gtk_list_item_get_item(item));
  if (row->channel && row->channel->entries.size()) {
    gtk_tree_expander_set_list_row(expander, GTK_TREE_LIST_ROW(row));
  }
  else if (row->entry) {
    auto label = GTK_LABEL(gtk_expander_get_child(GTK_EXPANDER(expander)));
    gtk_label_set_label(
      label,
      boost::lexical_cast<std::string>(row->entry->wdata.entryid).c_str());
    gtk_widget_set_tooltip_text(GTK_WIDGET(label),
                                row->entry->wdata.label.c_str());
  }
}

void ChannelOverviewGtk4::cbBindWriterId(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->entry) {
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_text(
      GTK_LABEL(label),
      boost::lexical_cast<std::string>(row->entry->wdata.clientid).c_str());
    // gtk_widget_set_tooltip_text(label, row->entry->wdata.clientid);
  }
}

void ChannelOverviewGtk4::cbBindES(GtkSignalListItemFactory *fact,
                                   GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->entry) {
    auto img = gtk_list_item_get_child(item);
    if (row->entry->wdata.eventtype) {
      gtk_image_set_from_paintable(GTK_IMAGE(img), GDK_PAINTABLE(event_icon));
    }
    else {
      gtk_image_set_from_paintable(GTK_IMAGE(img), GDK_PAINTABLE(stream_icon));
    }
  }
}

void ChannelOverviewGtk4::cbBindNWrites(GtkSignalListItemFactory *fact,
                                        GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->entry) {
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(
      GTK_LABEL(label),
      boost::lexical_cast<std::string>(row->entry->seq_id).c_str());
  }
}

void ChannelOverviewGtk4::cbBindReaderId(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->reader) {
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(
      GTK_LABEL(label),
      boost::lexical_cast<std::string>(row->reader->readerid).c_str());
  }
}

void ChannelOverviewGtk4::cbBindNReads(GtkSignalListItemFactory *fact,
                                       GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->reader) {
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(
      GTK_LABEL(label),
      boost::lexical_cast<std::string>(row->reader->seq_id).c_str());
  }
}

void ChannelOverviewGtk4::cbBindSel(GtkSignalListItemFactory *fact,
                                    GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->reader) {
    auto img = gtk_list_item_get_child(item);
    gtk_image_set_from_paintable(
      GTK_IMAGE(img),
      select_icon[min(row->reader->rdata.action, ChannelReadInfo::byLabel)]);
  }
}

void ChannelOverviewGtk4::cbBindSeq(GtkSignalListItemFactory *fact,
                                    GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->reader) {
    auto img = gtk_list_item_get_child(item);

    if (row->reader->rdata.sequential) {
      gtk_image_set_from_paintable(GTK_IMAGE(img), sequent_icon[0]);
    }
    else {
      gtk_image_set_from_paintable(GTK_IMAGE(img), sequent_icon[1]);
    }
  }
}

void ChannelOverviewGtk4::cbBindView(GtkSignalListItemFactory *fact,
                                     GtkListItem *item, gpointer user_data)
{
  auto row = D_CHANNEL_INFO(gtk_list_item_get_item(item));
  if (row->entry) {
    auto toggle = gtk_list_item_get_child(item);

    // property needs to point to the current row
    DChannelInfo *existing = NULL;
    g_object_get(G_OBJECT(toggle), "entry_row", &existing, NULL);
    if (existing != row) {
      g_object_set(G_OBJECT(toggle), "entry_row", row, NULL);
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
                                 row->entry->monitor &&
                                   row->entry->monitor->isOpen());
  }
}

DUECA_NS_END
