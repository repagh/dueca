import gi
gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gio, GObject, Gdk  # noqa
import cairo

home = ''

class Channel(GObject.GObject):
    def __init__(self, number: int, name: str, entries=None):
        super(Channel, self).__init__()
        self.number = number
        self.name = name
        self.children = entries

class Entry(GObject.GObject):
    def __init__(self, number: int, label: str, writerid: str, writername: str, 
                 es: bool, nwriters: int, readers=None):
        super(Entry, self).__init__()
        self.number = number
        self.label = label
        self.writerid = writerid
        self.writername = writername
        self.es = es
        self.nwrites = nwriters
        self.children = readers
        self.showing = False

class Reader(GObject.GObject):
    def __init__(self, readerid: str, readername: str, nreads: int, sel: int, seq: bool):
        super(Reader, self).__init__()
        self.readerid = readerid
        self.readername = readername
        self.nreads = nreads
        self.sel = sel
        self.seq = seq

def add_tree_node(item):
    
    if not (item):
            print("no item")
            return model
    else:        
        if type(item) == Gtk.TreeListRow:
            item = item.get_item()

            print("converteu")
            print(item)  
            
        if not item.children:
            return None
        if isinstance(item.children[0], Entry):
            store = Gio.ListStore.new(Entry)
        elif isinstance(item.childern[0], Reader):
            store = Gio.ListStore.new(Reader)
        for child in item.children:
            store.append(child)
        return store


# expanding column with channel number
def setup_expand(widget, item):
    """Setup the widget to show in the Gtk.Listview"""
    label = Gtk.Label()
    expander = Gtk.TreeExpander.new()
    expander.set_child(label)
    item.set_child(expander)

def setup_label(widget, item):
    """Setup the widget to show in the Gtk.Listview"""
    label = Gtk.Label()    
    item.set_child(label)

def setup_image(widget, item):
    image = Gtk.Image()
    item.set_child(image)

def setup_checkbox(widget, item):
    check = Gtk.CheckButton()
    item.set_child(check)

def bind_channelnum(widget, item):
    """bind data from the store object to the widget"""
    expander = item.get_child()
    label = expander.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Channel):
        label.set_label(str(obj.number))
        expander.set_list_row(row)
        expander.set_visible(True)
    else:
        expander.set_visible(False)

def bind_channelname(widget, item):
    """bind data from the store object to the widget"""
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Channel):
        label.set_label(obj.name)
        label.set_visible(True)
    else:
        label.set_visible(False)

def bind_entrynum(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        label.set_label(str(obj.number))
    else:
        label.set_label('')

def bind_writerid(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        label.set_label(str(obj.writerid))
        label.set_tooltip_text(obj.writername)
        label.set_visible(True)
    else:
        label.set_visible(False)

pb_E = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/event-logo.png'))
pb_S = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/stream-logo.png'))

def bind_es(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        if obj.es:
            image.set_from_paintable(pb_E)
        else:
            image.set_from_paintable(pb_S)
        image.set_visible(True)
    else:
        image.set_visible(False)

def bind_nwrites(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        label.set_label(str(obj.nwrites))
        label.set_visible(True)
    else:
        label.set_visible(False)

def bind_readers(widget, item):
    expander = item.get_child()
    label = expander.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        expander.set_list_row(row)
        label.set_label(obj.readerid)
        expander.set_visible(True)
    else:
        expander.set_visible(False)

def bind_readerid(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Reader):
        label.set_label(str(obj.readerid))
        label.set_tooltip_text(obj.readername)
    else:
        label.set_visible(False)

def bind_nreads(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Reader):
        label.set_label(str(obj.nreads))
    else:
        label.set_visible(False)
    
pb_label = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/label-logo.png'))
pb_number = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/number-logo.png'))
pb_multi = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/multi-logo.png'))

def bind_sel(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Reader):
        if obj.sel == 0:
            image.set_from_paintable(pb_number) 
        elif obj.sel == 1:
            image.set_from_paintable(pb_label) 
        elif obj.sel == 2:
            image.set_from_paintable(pb_multi) 
    else:
        image.set_visible(False)

pb_picking = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/picking-logo.png'))
pb_sequential = Gdk.Texture.new_from_file(
    Gio.File.new_for_path(f'{home}build-linux/pixmaps/sequential-logo.png'))

def bind_seq(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Reader):
        if obj.seq:
            image.set_from_paintable(pb_sequential) 
        else:
            image.set_from_paintable(pb_picking) 
    else:
        image.set_visible(False)

def bind_view(widget, item):
    check = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if isinstance(obj, Entry):
        check.set_visible(True)
        check.set_active(obj.showing)
    else:
        check.set_visible(False)
    
def on_activate(app):
    win = Gtk.ApplicationWindow(
        application=app,
        title="Gtk4 is Awesome !!!",
        default_height=400,
        default_width=400,
    )
    sw = Gtk.ScrolledWindow()
    list_view = Gtk.ColumnView()

    columns = []
    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_expand)
    f.connect("bind", bind_channelnum)
    columns.append(Gtk.ColumnViewColumn.new("chan #", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_label)
    f.connect("bind", bind_channelname)
    columns.append(Gtk.ColumnViewColumn.new("channel name", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_expand)
    f.connect("bind", bind_entrynum)
    columns.append(Gtk.ColumnViewColumn.new("entry #", f))
  
    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_label)
    f.connect("bind", bind_writerid)
    columns.append(Gtk.ColumnViewColumn.new("writer id", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_image)
    f.connect("bind", bind_es)
    columns.append(Gtk.ColumnViewColumn.new("E/S", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_label)
    f.connect("bind", bind_nwrites)
    columns.append(Gtk.ColumnViewColumn.new("#writes", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_expand)
    f.connect("bind", bind_readerid)
    columns.append(Gtk.ColumnViewColumn.new("reader id", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_label)
    f.connect("bind", bind_nreads)
    columns.append(Gtk.ColumnViewColumn.new("#reads", f))
  
    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_image)
    f.connect("bind", bind_sel)
    columns.append(Gtk.ColumnViewColumn.new("sel", f))

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_image)
    f.connect("bind", bind_seq)
    columns.append(Gtk.ColumnViewColumn.new("seq", f))
  
    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_checkbox)
    f.connect("bind", bind_view)
    columns.append(Gtk.ColumnViewColumn.new("view", f))
  
    selection = Gtk.SingleSelection()
    store = Gio.ListStore.new(Channel)
    model = Gtk.TreeListModel.new(store, False, False, add_tree_node)
    selection.set_model(model)
    list_view.set_model(selection)
    
    for c in columns:
        list_view.append_column(c)
    
    r1 = [Reader("id(0,23)", "eci://PHLAB", 4, 0, False), 
        Reader("id(0,24)", "hdf5-logger://PHLAB", 2, 1, True)]
    e1 = [Entry(0, "empty", "id(0,19)", "model-adapter://PHLAB", True, 20, r1)]
    store.append(Channel(29, "AircraftData://PHLAB", e1))
    store.append(Channel(30, "EmptyChannel://PHLAB"))
    sw.set_child(list_view)
    win.set_child(sw)
    win.present()


app = Gtk.Application(application_id="org.gtk.Example")
app.connect("activate", on_activate)
app.run(None)
