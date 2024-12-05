import gi
gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, GdkPixbuf, Gio, GObject  # noqa
import cairo


class Channel(GObject.GObject):
    def __init__(self, number: int, name: str, entries=None):
        super(Channel, self).__init__()
        self.number = number
        self.name = name
        self.children = entries

class Entry(GObject.GObject):
    def __init__(self, number: int, label: str, writerid: str, writername: str, 
                 es: bool, nwrites: int, readers=None):
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
        store = Gio.ListStore.new(DataObject)
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
    image = Gtk.Label()
    item.set_child(image)

def setup_checkbox(widget, item):
    check = Gtk.CheckButton()
    item.set_child(check)

def bind_channelnum(widget, item):
    """bind data from the store object to the widget"""
    expander = item.get_child()
    label = expander.get_child()
    row = item.get_item()
    expander.set_list_row(row)
    obj = row.get_item()
    label.set_label(obj.number)
    
def bind_channelname(widget, item):
    """bind data from the store object to the widget"""
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(obj.name)

def bind_entrynum(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(str(obj.number))

def bind_writerid(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(str(obj.writerid))
    label.set_tooltip_text(obj.writername)

pb_E = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/event-logo.png')
pb_S = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/stream-logo.png')

def bind_es(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if item.es:
        image.set_from_pixbuf(pb_E)
    else:
        image.set_from_pixbuf(pb_S)

def bind_nwrites(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(str(obj.nwrites))

def bind_readers(widget, item):
    expander = item.get_child()
    label = expander.get_child()
    row = item.get_item()
    expander.set_list_row(row)
    obj = row.get_item()
    label.set_label('')

def bind_readerid(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(str(obj.readerid))
    label.set_tooltip_text(obj.readername)

def bind_reads(widget, item):
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(str(obj.nreads))

pb_label = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/label-logo.png')
pb_number = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/number-logo.png')
pb_multi = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/multi-logo.png')

def bind_sel(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if obj.sel == 0:
       image.set_from_pixbuf(pb_number) 
    elif obj.sel == 1:
       image.set_from_pixbuf(pb_label) 
    elif obj.sel == 2:
       image.set_from_pixbuf(pb_multi) 

pb_picking = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/picking-logo.png')
pb_sequential = GdkPixbuf.Pixbuf.new_from_file(
    '../../build-linux/pixmaps/sequential-logo.png')

def bind_seq(widget, item):
    image = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    if obj.seq:
        image.set_from_pixbuf(pb_sequential) 
    else:
        image.set_from_pixbuf(pb_picking) 

def bind_view(widget, item):
    check = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    check.set_active(obj.showing)
    
def on_activate(app):
    win = Gtk.ApplicationWindow(
        application=app,
        title="Gtk4 is Awesome !!!",
        default_height=400,
        default_width=400,
    )
    sw = Gtk.ScrolledWindow()
    list_view = Gtk.ColumnView()

    factories = []
    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_expand)
    f.connect("bind", bind_expand)
    factories.append(f)

    f = Gtk.SignalListItemFactory()
    f.connect("setup", setup_label)
    f.connect("bind", bind_channelnum)
    
    
    selection = Gtk.SingleSelection()
    
    store = Gio.ListStore.new(Channel)
    
    model = Gtk.TreeListModel.new(store, False, False, add_tree_node)
    
    selection.set_model(model)
    
    list_view.set_model(selection)
    
    columns = [
        Gtk.ColumnViewColumn.new("chan #", factory),
        Gtk.ColumnViewColumn.new("channel name", factory1),
        Gtk.ColumnViewColumn.new("entry #", factory2),
        Gtk.ColumnViewColumn.new("writer id", factory3),
        Gtk.ColumnViewColumn.new("E/S", factory4),
        Gtk.ColumnViewColumn.new("#writes", factory5),
        Gtk.ColumnViewColumn.new("reader id", factory6),
        Gtk.ColumnViewColumn.new("#reads", factory7),
        Gtk.ColumnViewColumn.new("sel", factory8),
        Gtk.ColumnViewColumn.new("seq", factory9),
        Gtk.ColumnViewColumn.new("view", factory10)
    ]
    for c in columns:
        list_view.append_column(c)
    
    r1 = [Reader("id(0,23)", "eci://PHLAB", 4, 0, False), 
        Reader("id(0,24)", "hdf5-logger://PHLAB", 2, 1, True)]
    e1 = [Entry(0, "empty", "id(0,19)", "model-adapter://PHLAB", True, 20, r1)]
    store.append(Channel(29, "AircraftData://PHLAB", e1))

    sw.set_child(list_view)
    win.set_child(sw)
    win.present()


app = Gtk.Application(application_id="org.gtk.Example")
app.connect("activate", on_activate)
app.run(None)
