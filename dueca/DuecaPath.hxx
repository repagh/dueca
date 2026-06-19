/* ------------------------------------------------------------------   */
/*      item            : DuecaPath.hxx
        made by         : Rene van Paassen
        date            : 010817
        category        : header file
        description     :
        changes         : 010817 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef DuecaPath_hxx
#define DuecaPath_hxx

#include <stringoptions.h>

#include <dueca_ns.h>
namespace dueca {
/** class that makes it possible to find the dueca data files. */
class DuecaPath
{
  /** Path to the dueca installation directory. */
  vstring basepath;

  /** Constructor. */
  DuecaPath();

  /// Destructor
  ~DuecaPath();

  /// singleton pointer.
  static DuecaPath* singleton;

  /// to prevent complaints
  friend class Nobody;
public:

  /** Take a tail, e.g. a filename or directory + filename, and
      prepend the current DUECA data path to it. */
  static vstring prepend(const char* tail);

  /** Take a tail, e.g. a filename or directory + filename, and
      prepend the current DUECA data path to it. */
  static vstring prepend(const vstring& tail);
};

} // namespace dueca
#endif
