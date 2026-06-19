/* ------------------------------------------------------------------   */
/*      item            : GtkOpenGLHelper.hxx
        made by         : Rene van Paassen
        date            : 060531
        category        : header file
        description     : Common interface class to glut, gtk or
                          whatever-based opengl windowing code
        changes         : 060531 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef GtkOpenGLHelper_hxx
#define GtkOpenGLHelper_hxx

#include "GtkGLWidgetHelper.hxx"
namespace dueca {

/** This class defines the basic opengl operations. Derived classes
    implement these for specific windowing interfaces. */

class GtkGLWindowHelper: public GtkGLWidgetHelper
{
  /** Window id in GTK. */
  GtkWidget* gtk_win_id;

public:
  /** Hide the window. */
  void hide();
  /** Show the window. */
  void show();
  /** Constructor. */
  GtkGLWindowHelper();
  /** Open call. */
  void open(const std::string &title, DuecaGLWindow* master,
            int offset_x, int offset_y, int width, int height,
            bool fullscreen = false);

  /** Delete the window. */
  ~GtkGLWindowHelper();
};

} // namespace dueca

#endif
