import gi
gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gio, GObject  # noqa


class Channel(GObject.GObject):
    def __init__(self, number: int, name: str, entries=None):
        super(Channel, self).__init__()
        self.number = number
        self.name = name
        self.children = entries

class Entry(Gobject.Gobject):
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

class Reader(Gobject.Gobject):
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


def setup(widget, item):
    """Setup the widget to show in the Gtk.Listview"""
    label = Gtk.Label()
    expander = Gtk.TreeExpander.new()
    expander.set_child(label)
    item.set_child(expander)


def bind(widget, item):
    """bind data from the store object to the widget"""
    expander = item.get_child()
    label = expander.get_child()
    row = item.get_item()
    expander.set_list_row(row)
    obj = row.get_item()
    label.set_label(obj.data)
    
def setup1(widget, item):
    """Setup the widget to show in the Gtk.Listview"""
    label = Gtk.Label()    
    item.set_child(label)


def bind1(widget, item):
    """bind data from the store object to the widget"""
    label = item.get_child()
    row = item.get_item()
    obj = row.get_item()
    label.set_label(obj.data2)


def on_activate(app):
    win = Gtk.ApplicationWindow(
        application=app,
        title="Gtk4 is Awesome !!!",
        default_height=400,
        default_width=400,
    )
    sw = Gtk.ScrolledWindow()
    list_view = Gtk.ColumnView()  
    factory = Gtk.SignalListItemFactory()
    factory.connect("setup", setup)
    factory.connect("bind", bind)
    
    factory1 = Gtk.SignalListItemFactory()
    factory1.connect("setup", setup1)
    factory1.connect("bind", bind1)
    
    
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
