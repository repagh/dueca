/* ------------------------------------------------------------------   */
/*      item            : @Module@.hxx
        made by         : @author@
        from template   : DuecaHelperTemplate.hxx (2026.04)
        date            : @date@
        category        : header file
        description     :
        changes         : @date@ first version
        language        : C++
        copyright       : (c)
*/

#pragma once

// include the dueca header
#include <dueca/ScriptCreatable.hxx>
#include <dueca/stringoptions.h>
#include <dueca/ParameterTable.hxx>
#include <dueca/dueca_ns.h>

/** A class definition for a DUECA helper class

    This class has been derived from the ScriptCreatable base class,
    and has a (Scheme or Python) script command to create it and
    optionally add parameters.

    The instructions to create an object of this class from the start
    script are:

    \verbinclude @smodule@.scm
 */
class @Module@: public dueca::ScriptCreatable
{
private: // simulation data

public: // construction and further specification
  /** Constructor. Is normally called from the creation script. */
  @Module@();

  /** Continued construction. This is called after all script
      parameters have been read and filled in, according to the
      parameter table. */
  bool complete();

  /** Destructor. */
  ~@Module@();

  /** Obtain a pointer to the parameter table. */
  static const dueca::ParameterTable* getParameterTable();

public:

};
