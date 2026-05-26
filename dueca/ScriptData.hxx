/* ------------------------------------------------------------------   */
/*      item            : ScriptData.hh
        made by         : Rene' van Paassen
        date            : 990709
        category        : header file
        description     :
        changes         : 990709 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef ScriptData_hh
#define ScriptData_hh

#include "ScriptObject.hxx"
#include <list>
#include "dueca-guile.h"
#include <sstream>
#include <dueca_ns.h>
namespace dueca {

/** \file
    Header with classes and macros to make a class accessible from Scheme. */
#define SCM_FEATURES_DEF

/** A shortcut macro for making a class creatable from Scheme. */
/* #define SCM_FEATURES_DEF
   _Pragma ("GCC warning \"'SCM_FEATURES_DEF' macro is deprecated\"" */

} // namespace dueca
#endif
