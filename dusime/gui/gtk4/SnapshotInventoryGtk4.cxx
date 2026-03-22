/* ------------------------------------------------------------------   */
/*      item            : SnapshotInventoryGtk4.cxx
        made by         : repa
        from template   : DuecaModuleTemplate.cxx
        template made by: Rene van Paassen
        date            : Mon May  2 17:16:41 2022
        category        : body file
        description     :
        changes         : Mon May  2 17:16:41 2022 first version
        template changes: 030401 RvP Added template creation comment
                          060512 RvP Modified token checking code
                          160511 RvP Some comments updated
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#include "glib-object.h"
#define SnapshotInventoryGtk4_cxx

// include the definition of the module class
#include "SnapshotInventoryGtk4.hxx"
#include <dueca/DuecaPath.hxx>
#define DEBPRINTLEVEL -1
#include <debprint.h>
#include <dueca/gui/gtk4/GtkDuecaView.hxx>
#include <boost/date_time/posix_time/posix_time.hpp>

// include the debug writing header, by default, write warning and
// error messages
#define W_MOD
#define E_MOD
#include <debug.h>

// include additional files needed for your calculation here

// the standard package for DUSIME, including template source
#define DO_INSTANTIATE
#define NO_TYPE_CREATION
#include <dueca/dueca.h>

namespace {

// define the datatype to be held in the store, top level
struct _DSnapShotSet
{
  GObject parent;
  dueca::SnapshotInventory::snapmap_t::const_iterator data;
  GListStore *children;
};

G_DECLARE_FINAL_TYPE(DSnapShotSet, d_snap_shot_set, D, SNAP_SHOT_SET, GObject);
G_DEFINE_TYPE(DSnapShotSet, d_snap_shot_set, G_TYPE_OBJECT);

// level below, individual snapshots
struct _DSnapShot
{
  GObject parent;
  std::list<dueca::Snapshot>::const_iterator data;
};

G_DECLARE_FINAL_TYPE(DSnapShot, d_snap_shot, D, SNAP_SHOT, GObject);
G_DEFINE_TYPE(DSnapShot, d_snap_shot, G_TYPE_OBJECT);

static void d_snap_shot_class_init(DSnapShotClass *klass) {}
static void d_snap_shot_init(DSnapShot *self) {}

static void d_snap_shot_set_class_init(DSnapShotSetClass *klass) {}
static void d_snap_shot_set_init(DSnapShotSet *self) {}

static DSnapShotSet *d_snap_shot_set_new(
  const dueca::SnapshotInventory::snapmap_t::const_iterator &data)
{
  auto res = D_SNAP_SHOT_SET(g_object_new(d_snap_shot_set_get_type(), NULL));
  res->data = data;
  return res;
}

static DSnapShot *d_snap_shot_new(const std::list<Snapshot>::const_iterator &ii)
{
  auto res = D_SNAP_SHOT(g_object_new(d_snap_shot_get_type(), NULL));
  res->data = ii;
  return res;
}

static void d_snap_shop_set_unref_children(gpointer _item, GObject *oldlist)
{
  auto item = D_SNAP_SHOT_SET(_item);
  item->children = NULL;
}

static GListModel *add_data_element(gpointer _item, gpointer user_data)
{
  auto item = D_SNAP_SHOT_SET(_item);
  assert(item->children == NULL);
  auto lm = g_list_store_new(d_snap_shot_get_type());
  for (auto c = item->data->second.snaps.begin();
       c != item->data->second.snaps.end(); c++) {
    auto child = d_snap_shot_new(c);
    g_list_store_append(lm, gpointer(child));
    g_object_unref(child);
  }
  g_object_weak_ref(G_OBJECT(lm), d_snap_shop_set_unref_children, item);
  item->children = lm;
  return G_LIST_MODEL(lm);
}

} // end anonymous namespace

DUECA_NS_START;

// class/module name
const char *const SnapshotInventoryGtk4::classname = "initials-inventory";

// Parameters to be inserted
const ParameterTable *SnapshotInventoryGtk4::getParameterTable()
{
  static const ParameterTable parameter_table[] = {

    { "glade-file",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::gladefile),
      "Interface description (glade, gtkbuilder) for the channel view window" },

    { "position-size",
      new MemberCall<_ThisModule_, std::vector<int>>(
        &_ThisModule_::setPositionAndSize),
      "Specify the position, and optionally also the size of the interface\n"
      "window." },

    { "reference-file",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::reference_file),
      "File with existing initial states (snapshots). Will be read and\n"
      "used to populate the initial set" },

    { "store-file",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::store_file),
      "When additional snapshots are taken in this simulation, these will\n"
      "be written in this file, together with the existing initial state\n"
      "sets. Uses a template, check check boost time_facet for format\n"
      "strings. Default \"\", suggestion\n"
      "initial-[entity name]-%Y%m%d_%H%M%S.toml" },

    { "path",
      new VarProbe<_ThisModule_, std::string>(&_ThisModule_::store_path),
      "Path for storing and retrieving initial state files." },

    { NULL, NULL, "Manage loading of initial states (snapshots). " }
  };

  return parameter_table;
}

// constructor
SnapshotInventoryGtk4::SnapshotInventoryGtk4(Entity *e, const char *part,
                                             const PrioritySpec &ps) :
  /* The following line initialises the SimulationModule base class.
     You always pass the pointer to the entity, give the classname and the
     part arguments. */
  Module(e, classname, part),

  // initialize the data you need in your simulation or process
  gladefile(DuecaPath::prepend("initials_inventory-gtk4.ui")),
  window(),
  editwin(NULL),
  snaps_store(NULL),
  menuaction(NULL),
  reference_file(),
  store_file(),
  store_path(),
  editmap(),
  sel_origin(editmap.end()),
  editing_snap()
{
  //
}

static std::string formatTime(const boost::posix_time::ptime &now,
                              const std::string &lft)
{
  using namespace boost::posix_time;
  std::locale loc(std::cout.getloc(), new time_facet(lft.c_str()));

  std::basic_stringstream<char> wss;
  wss.imbue(loc);
  wss << now;
  return wss.str();
}

bool SnapshotInventoryGtk4::complete()
{
  /* All your parameters have been set. You may do extended
     initialisation here. Return false if something is wrong. */
  if (getPart().size() == 0) {
    /* DUSIME replay&initial

       Specify the entity to be managed in the part name. */
    E_XTR("Supply the managed entity to the snapshot inventory");
    return false;
  }

  // find the underlying inventory
  inventory = SnapshotInventory::findSnapshotInventory(getPart());

  // if applicable, open the files
  inventory->setFiles(
    reference_file,
    formatTime(boost::posix_time::second_clock::local_time(), store_file),
    store_path);

  // table with callbacks to be connected to widget actions
  static GladeCallbackTable cb_table[] = {
    { "initials_close", "clicked", gtk_callback(&_ThisModule_::cbClose) },
    { "initials_newentryname", "changed",
      gtk_callback(&_ThisModule_::cbSetName) },
    { "initials_send", "clicked", gtk_callback(&_ThisModule_::cbSendInitial) },
    { "initials_edit", "clicked", gtk_callback(&_ThisModule_::cbEditInitial) },
    { "initials_view", "close-request", gtk_callback(&_ThisModule_::cbDelete) },
    { "initials_name_fact", "setup",
      gtk_callback(&_ThisModule_::cbSetupExpander) },
    { "initials_datetime_fact", "setup",
      gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "initials_origin_fact", "setup",
      gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "initials_coding_fact", "setup",
      gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "initials_sample_fact", "setup",
      gtk_callback(&_ThisModule_::cbSetupLabel) },
    { "initials_name_fact", "bind", gtk_callback(&_ThisModule_::cbBindName) },
    { "initials_datetime_fact", "bind",
      gtk_callback(&_ThisModule_::cbBindDateTime) },
    { "initials_origin_fact", "bind",
      gtk_callback(&_ThisModule_::cbBindOrigin) },
    { "initials_coding_fact", "bind",
      gtk_callback(&_ThisModule_::cbBindCoding) },
    { "initials_sample_fact", "bind",
      gtk_callback(&_ThisModule_::cbBindSample) },
    { "editwin_revert", "clicked", gtk_callback(&_ThisModule_::cbEditRevert) },
    { "editwin_update", "clicked", gtk_callback(&_ThisModule_::cbEditUpdate) },
    { "editwin_module_selection", "notify::selected-item",
      gtk_callback(&_ThisModule_::cbEditSelection) },
    { "initials_view", "close-request", gtk_callback(&_ThisModule_::cbDelete) },
    { "editwin", "close-request", gtk_callback(&_ThisModule_::cbEditDelete) },
    { "editwin_close", "clicked", gtk_callback(&_ThisModule_::cbEditClose) },

    { NULL }
  };

  // create the window
  bool res = window.readGladeFile(gladefile.c_str(), "initials_view",
                                  reinterpret_cast<gpointer>(this), cb_table);
  if (!res) {
    /* DUECA UI.

       Cannot find the glade file defining the initial state view
       GUI. Check DUECA installation and paths.
    */
    E_CNF("failed to open initials overview " << gladefile);
    return res;
  }

  // find the store for the snapshot data, and fill
  GtkColumnView *treeview = GTK_COLUMN_VIEW(window["initials_initiallist"]);
  snaps_store = g_list_store_new(d_snap_shot_set_get_type());
  auto model = gtk_tree_list_model_new(G_LIST_MODEL(snaps_store), FALSE, FALSE,
                                       add_data_element, NULL, NULL);
  auto selection = gtk_single_selection_new(G_LIST_MODEL(model));
  gtk_single_selection_set_autoselect(selection, FALSE);
  gtk_single_selection_set_can_unselect(selection, TRUE);
  auto cb = gtk_callback(&_ThisModule_::cbSelection, this);
  g_signal_connect(selection, "selection-changed", cb->callback(), cb);
  gtk_column_view_set_model(treeview, GTK_SELECTION_MODEL(selection));

  // it is a tree structure, base element for each snapshot set, with
  // leafs for the different snapshots

  // load the tree with the currently present data
  for (auto isn = inventory->getSnapshotData().begin();
       isn != inventory->getSnapshotData().end(); isn++) {

    auto nset = d_snap_shot_set_new(isn);
    g_list_store_append(snaps_store, nset);
  }

  // create a callback for getting any new incoming snapshot sets
  inventory->informOnNewSet(
    [this](const std::string &name,
           const SnapshotInventory::SnapshotData &snapset) {
      auto newset = inventory->getSnapshotData().find(name);
      auto nset = d_snap_shot_set_new(newset);
      g_list_store_append(snaps_store, nset);
    });

  inventory->informOnNewSnap([this](const Snapshot &snap) {
    auto n = g_list_model_get_n_items(G_LIST_MODEL(snaps_store));
    auto lset =
      D_SNAP_SHOT_SET(g_list_model_get_item(G_LIST_MODEL(snaps_store), n - 1));
    if (lset->children) {
      auto lastsnap = std::prev(lset->data->second.snaps.end());
      g_list_store_append(lset->children, d_snap_shot_new(lastsnap));
    }
  });

  // set a title
  gtk_window_set_title(
    GTK_WINDOW(window["initials_view"]),
    (std::string("Initials control - ") + getPart()).c_str());

  editwin = window["editwin"];
  gtk_window_set_title(GTK_WINDOW(editwin),
                       (std::string("Initials edit - ") + getPart()).c_str());

  // insert in DUECA's menu
  menuaction = GtkDuecaView::single()->requestViewEntry(
    (std::string("initial_") + getPart()).c_str(),
    (std::string("Initial state - ") + getPart()).c_str(),
    G_OBJECT(window["initials_view"]));

  return res;
}

// destructor
SnapshotInventoryGtk4::~SnapshotInventoryGtk4()
{
  //
}

// tell DUECA you are prepared
bool SnapshotInventoryGtk4::isPrepared()
{
  bool res = true;

  // return result of checks
  return res;
}

// start the module
void SnapshotInventoryGtk4::startModule(const TimeSpec &time)
{
  // do_calc.switchOn(time);
}

// stop the module
void SnapshotInventoryGtk4::stopModule(const TimeSpec &time)
{
  // do_calc.switchOff(time);
}

// callbacks to link to the gui
void SnapshotInventoryGtk4::cbClose(GtkWidget *button, gpointer gp)
{
  GtkDuecaView::toggleView(menuaction);
  // g_signal_emit_by_name(G_OBJECT(menuaction), "activate", NULL);
}

void SnapshotInventoryGtk4::cbSetName(GtkWidget *text, gpointer gp)
{
  inventory->setSnapName(gtk_editable_get_chars(GTK_EDITABLE(text), 0, -1));
}

void SnapshotInventoryGtk4::cbSendInitial(GtkWidget *btn, gpointer gp)
{
  if (inventory->sendSelected()) {
    gtk_label_set_text(GTK_LABEL(window["initials_status"]), "loaded");
    // gtk_widget_set_sensitive(GTK_WIDGET(window["initials_send"]), FALSE);
    // gtk_tree_selection_unselect_all(GTK_TREE_SELECTION
    //  (window["initials_listselection"]));
  }
  else {
    gtk_label_set_text(GTK_LABEL(window["initials_status"]), "send failed");
  }
}

void SnapshotInventoryGtk4::cbSelection(GtkSelectionModel *sel, guint position,
                                        guint n_items, gpointer gp)
{
  if (gtk_selection_model_is_selected(sel, position)) {

    auto it = D_SNAP_SHOT_SET(
      g_list_model_get_item(G_LIST_MODEL(snaps_store), position));
    assert(inventory->changeSelection(it->data->first.c_str()));

    gtk_label_set_text(GTK_LABEL(window["initials_selected"]),
                       it->data->first.c_str());
    gtk_widget_set_sensitive(GTK_WIDGET(window["initials_send"]), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(window["initials_edit"]), TRUE);
    gtk_label_set_text(GTK_LABEL(window["initials_status"]), "selected");
  }
  else {

    gtk_label_set_text(GTK_LABEL(window["initials_selected"]),
                       "<< none selected >>");
    gtk_widget_set_sensitive(GTK_WIDGET(window["initials_send"]), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(window["initials_edit"]), FALSE);
    gtk_label_set_text(GTK_LABEL(window["initials_status"]), "");
  }
}

gboolean SnapshotInventoryGtk4::cbDelete(GtkWidget *window, gpointer user_data)
{
  // fixes the menu check, and closes the window
  // g_signal_emit_by_name(G_OBJECT(menuaction), "activate", NULL);
  GtkDuecaView::toggleView(menuaction);
  // indicate that the event is handled
  return TRUE;
}

bool SnapshotInventoryGtk4::setPositionAndSize(const std::vector<int> &p)
{
  if (p.size() == 2 || p.size() == 4) {
    window.setWindow(p);
  }
  else {
    /* DUECA UI.

         Window setting needs 2 (for size) or 4 (also location)
         arguments. */
    E_CNF(getId() << '/' << classname << " need 2 or 4 arguments");
    return false;
  }
  return true;
}

void SnapshotInventoryGtk4::cbSetupLabel(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto label = gtk_label_new("");
  gtk_list_item_set_child(item, label);
}

void SnapshotInventoryGtk4::cbSetupExpander(GtkSignalListItemFactory *fact,
                                            GtkListItem *item,
                                            gpointer user_data)
{
  auto label = gtk_label_new("");
  auto expander = gtk_tree_expander_new();
  gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander), label);
  gtk_list_item_set_child(item, expander);
}

void SnapshotInventoryGtk4::cbBindName(GtkSignalListItemFactory *fact,
                                       GtkListItem *item, gpointer user_data)
{
  auto expander = GTK_TREE_EXPANDER(gtk_list_item_get_child(item));
  auto row = gtk_list_item_get_item(item);
  auto _snap = gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  if (D_IS_SNAP_SHOT_SET(_snap)) {
    auto snap = D_SNAP_SHOT_SET(_snap);
    auto label = gtk_tree_expander_get_child(expander);
    gtk_tree_expander_set_list_row(expander, GTK_TREE_LIST_ROW(row));
    gtk_label_set_label(GTK_LABEL(label), snap->data->first.c_str());
  }
}

void SnapshotInventoryGtk4::cbBindDateTime(GtkSignalListItemFactory *fact,
                                           GtkListItem *item,
                                           gpointer user_data)
{
  auto row = gtk_list_item_get_item(item);
  auto _snap = gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  if (D_IS_SNAP_SHOT_SET(_snap)) {
    auto snap = D_SNAP_SHOT_SET(_snap);
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(GTK_LABEL(label),
                        snap->data->second.getTimeLocal().c_str());
  }
}

void SnapshotInventoryGtk4::cbBindOrigin(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = gtk_list_item_get_item(item);
  auto _snap = gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  if (D_IS_SNAP_SHOT(_snap)) {
    auto snap = D_SNAP_SHOT(_snap);
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(GTK_LABEL(label), snap->data->originator.name.c_str());
  }
}

void SnapshotInventoryGtk4::cbBindCoding(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = gtk_list_item_get_item(item);
  auto _snap = gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  if (D_IS_SNAP_SHOT(_snap)) {
    auto snap = D_SNAP_SHOT(_snap);
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(GTK_LABEL(label), getString(snap->data->coding));
  }
}

void SnapshotInventoryGtk4::cbBindSample(GtkSignalListItemFactory *fact,
                                         GtkListItem *item, gpointer user_data)
{
  auto row = gtk_list_item_get_item(item);
  auto _snap = gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  if (D_IS_SNAP_SHOT(_snap)) {
    auto snap = D_SNAP_SHOT(_snap);
    auto label = gtk_list_item_get_child(item);
    gtk_label_set_label(GTK_LABEL(label), snap->data->getSample().c_str());
  }
}

// editing code

static void cbBufferChanged(GtkTextBuffer *buf, gpointer gp)
{
  reinterpret_cast<SnapshotInventoryGtk4 *>(gp)->cbEditChange(buf);
}

void SnapshotInventoryGtk4::prepareEditingMap(bool init)
{
  if (init) {
    editing_snap = inventory->getSelected();
  }

  assert(inventory->haveIncoSet(editing_snap));

  // list of tabs/labels
  std::list<std::string> origin_list;

  // has the list been expanded or changed
  bool changes = false;

  auto &snapset = inventory->editSnapshot(editing_snap);
  for (auto &ed : editmap) {
    ed.second.todelete = true;
  }

  for (const auto &sn : snapset.snaps) {

    // add an edit page if the snapshot is of the editable type
    if (sn.coding != Snapshot::UnSpecified && sn.coding != Snapshot::Base64 &&
        sn.coding != Snapshot::Base64File &&
        sn.coding != Snapshot::BinaryFile) {

      auto edit = editmap.find(sn.originator.name);
      origin_list.push_back(sn.originator.name);

      if (edit == editmap.end()) {

        changes = true;
        auto ne = editmap.emplace(
          std::piecewise_construct, std::forward_as_tuple(sn.originator.name),
          std::forward_as_tuple(gtk_text_buffer_new(NULL),
                                gtk_text_buffer_new(NULL)));
        g_signal_connect(G_OBJECT(ne.first->second.edit_text), "changed",
                         G_CALLBACK(cbBufferChanged), this);
        DEB("New buf snapshot " << reinterpret_cast<const void *>(&sn) << " "
                                << sn.originator.name);

        std::string etext(sn.getEdit());
        gtk_text_buffer_set_text(ne.first->second.edit_text, etext.c_str(),
                                 etext.size());
        // labels?
        std::ifstream f(store_path + std::string("/") +
                        sn.originator.getClass() + std::string("-labels.txt"));
        if (f.good()) {
          std::string labels(std::istreambuf_iterator<char>{ f }, {});
          gtk_text_buffer_set_text(ne.first->second.edit_labels, labels.c_str(),
                                   labels.size());
        }
        else {
          std::stringstream labels;
          for (auto n = 0U; n < std::count(etext.begin(), etext.end(), '\n');
               n++) {
            labels << (n + 1) << std::endl;
          }
          gtk_text_buffer_set_text(ne.first->second.edit_labels,
                                   labels.str().c_str(), labels.str().size());
        }
      }
      else {
        // re-use, refresh this text buffer
        edit->second.todelete = false;
        if (init || edit->second.dirty) {
          edit->second.dirty = false;
          DEB("Update snapshot " << reinterpret_cast<const void *>(&sn) << " "
                                 << sn.originator.name);

          std::string etext(sn.getEdit());
          gtk_text_buffer_set_text(edit->second.edit_text, etext.c_str(),
                                   etext.size());
        }
      }
    }
  }

  // check if any buffers need deleting
  for (auto emit = editmap.begin(); emit != editmap.end();) {
    if (emit->second.todelete) {
      emit = editmap.erase(emit);
      changes = true;
    }
    else {
      emit++;
    }
  }

  // update and revert no longer sensitive
  gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_update"]), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_revert"]), FALSE);

  // when the buffer list changed, or a new inco set loaded, reload the combo,
  // and select the first set
  if (init || changes) {
    // reload the combobox options
    window.loadDropDownText("editwin_module_selection", origin_list);
    window.setValue(origin_list.front(), "editwin_module_selection");
  }
}

void SnapshotInventoryGtk4::readEditingMap()
{
  auto &snapset = inventory->editSnapshot(editing_snap);

  for (auto &sn : snapset.snaps) {

    auto mapit = editmap.find(sn.originator.name);
    if (mapit != editmap.end() && mapit->second.dirty) {
      DEB("Data snapshot " << reinterpret_cast<void *>(&sn) << " "
                           << sn.originator.name);
      GtkTextIter tstart, tend;
      gtk_text_buffer_get_start_iter(mapit->second.edit_text, &tstart);
      gtk_text_buffer_get_end_iter(mapit->second.edit_text, &tend);
      sn.setEdit(gtk_text_buffer_get_text(mapit->second.edit_text, &tstart,
                                          &tend, FALSE));
      mapit->second.dirty = false;
    }
  }
  gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_update"]), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_revert"]), FALSE);
}

void SnapshotInventoryGtk4::cbEditInitial(GtkWidget *btn, gpointer gp)
{
  prepareEditingMap(true);
  gtk_widget_set_visible(editwin, TRUE);
}

void SnapshotInventoryGtk4::cbEditRevert(GtkWidget *btn, gpointer gp)
{
  // reload all original text buffers
  prepareEditingMap(false);
}

void SnapshotInventoryGtk4::cbEditUpdate(GtkWidget *btn, gpointer gp)
{
  readEditingMap();
}

void SnapshotInventoryGtk4::cbEditChange(GtkTextBuffer *tb)
{
  if (sel_origin != editmap.end()) {
    if (!sel_origin->second.dirty) {
      gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_update"]), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(window["editwin_revert"]), TRUE);

      sel_origin->second.dirty = true;
    }
  }
}

SnapshotInventoryGtk4::EditData::EditData(GtkTextBuffer *edit_text,
                                          GtkTextBuffer *edit_labels) :
  edit_text(edit_text),
  edit_labels(edit_labels),
  dirty(false),
  todelete(false)
{
  if (edit_text)
    g_object_ref(edit_text);
  if (edit_labels)
    g_object_ref(edit_labels);
}

SnapshotInventoryGtk4::EditData::~EditData()
{
  if (edit_text)
    g_object_unref(edit_text);
  if (edit_labels)
    g_object_unref(edit_labels);
}

void SnapshotInventoryGtk4::cbEditSelection(GObject *box, GParamSpec *pspec,
                                            gpointer gp)
{
  std::string selorigin;
  window.getValue(selorigin, "editwin_module_selection");
  sel_origin = editmap.find(selorigin);
  if (sel_origin == editmap.end()) {
    W_XTR("Somehow cannot find edit selection");
    return;
  }
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(window["editwin_inco"]),
                           sel_origin->second.edit_text);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(window["editwin_linenumbers"]),
                           sel_origin->second.edit_labels);
}

gboolean SnapshotInventoryGtk4::cbEditDelete(GtkWidget *window, GdkEvent *event,
                                             gpointer user_data)
{
  // fixes the menu check, and closes the window
  gtk_widget_set_visible(editwin, FALSE);

  // indicate that the event is handled
  return TRUE;
}

void SnapshotInventoryGtk4::cbEditClose(GtkWidget *button, gpointer gp)
{
  gtk_widget_set_visible(editwin, FALSE);
}

DUECA_NS_END;
