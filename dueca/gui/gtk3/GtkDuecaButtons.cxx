/* ------------------------------------------------------------------   */
/*      item            : GtkDuecaButtons.cxx
        made by         : Rene' van Paassen
        date            : 060424
        category        : body file
        description     :
        changes         : 060424 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define GtkDuecaButtons_cxx
#include "GtkDuecaButtons.hxx"

#include <DuecaPath.hxx>
#include <debug.h>

static GdkPixbuf *emergency_icons[2] = { NULL, NULL };
static GdkPixbuf *button_icons[5] = { NULL, NULL, NULL };

static GdkPixbuf *loadTextureFromFile(const char *fname)
{
  GError *error = NULL;
  auto tex =
    gdk_pixbuf_new_from_file(dueca::DuecaPath::prepend(fname).c_str(), &error);
  if (error) {
    /* DUECA UI.

       Cannot load icon texture. Check DUECA installation.
    */
    E_XTR("Could not load texture \"" << dueca::DuecaPath::prepend(fname)
                                      << "\": " << error->message);
    g_error_free(error);
    error = NULL;
    return NULL;
  }
  g_object_ref(G_OBJECT(tex));
  return GDK_PIXBUF(tex);
}

static void toggle_visibility(GtkWidget *widget, gpointer data)
{
  if ((*((int *)data))-- == 0) {
    gtk_widget_show(widget);
  }
  else {
    gtk_widget_hide(widget);
  }
}

void gtk_dueca_button_set_image(GtkWidget *widget, gint imgno)
{
  g_return_if_fail(widget != NULL);
  g_return_if_fail(GTK_IS_CONTAINER(widget));
  gtk_container_foreach(GTK_CONTAINER(gtk_bin_get_child(GTK_BIN(widget))),
                        toggle_visibility, (gpointer)&imgno);
}

GtkWidget *NewGtkDuecaButton_pixmaps()
{
  static bool loaded = false;
  if (!loaded) {
    button_icons[0] = loadTextureFromFile("pixmaps/inactive.png");
    button_icons[1] = loadTextureFromFile("pixmaps/inprogress.png");
    button_icons[2] = loadTextureFromFile("pixmaps/active.png");
    button_icons[3] = loadTextureFromFile("pixmaps/incomplete.png");
    button_icons[4] = loadTextureFromFile("pixmaps/fell-back.png");
    loaded = true;
  }

  //GdkBitmap *mask = NULL;
  //cairo_surface_t *pm;
  GtkWidget *pixmap;
  GtkContainer *box = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  // inactive pixmap
  //pm = cairo_image_surface_create_from_xpm
  //  (DUECA_NS ::DuecaPath::prepend("pixmaps/inactive.xpm").c_str());

  pixmap = gtk_image_new_from_pixbuf(button_icons[0]);
  gtk_container_add(box, pixmap);
  gtk_widget_show(pixmap);

   // progress pixmap
  pixmap = gtk_image_new_from_pixbuf(button_icons[1]);
  gtk_container_add(box, pixmap);

  // final pixmap
  pixmap = gtk_image_new_from_pixbuf(button_icons[2]);
  gtk_container_add(box, pixmap);

  // broken consistency pixmap
  pixmap = gtk_image_new_from_pixbuf(button_icons[3]);
  gtk_container_add(box, pixmap);

  // fallback pixmap
  pixmap = gtk_image_new_from_pixbuf(button_icons[4]);
  gtk_container_add(box, pixmap);

  // show the box
  gtk_widget_show(GTK_WIDGET(box));

  return GTK_WIDGET(box);
}

GtkWidget *NewGtkDuecaAbortButton_pixmaps()
{
  // load the emergency and button icons
  static bool loaded = false;
  if (!loaded) {
    emergency_icons[0] = loadTextureFromFile("pixmaps/abort.png");
    emergency_icons[1] = loadTextureFromFile("pixmaps/confirm-abort.png");
    loaded = true;
  }

  // make a container with the many faces of this button
  GtkContainer *box = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

  // Abort pixmap
  GtkWidget *pixmap;

  pixmap = gtk_image_new_from_pixbuf(emergency_icons[0]);
  gtk_container_add(box, pixmap);
  gtk_widget_show(pixmap);

   // Abort confirmation
  pixmap = gtk_image_new_from_pixbuf(emergency_icons[1]);
  gtk_container_add(box, pixmap);
  gtk_widget_show(GTK_WIDGET(box));

  return GTK_WIDGET(box);
}
