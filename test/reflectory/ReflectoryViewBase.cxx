/* ------------------------------------------------------------------   */
/*      item            : ReflectoryViewBase.cxx
        made by         : Rene' van Paassen
        date            : 160928
        category        : body file
        description     :
        changes         : 160928 first version
        language        : C++
        copyright       : (c) 16 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define ReflectoryViewBase_cxx
#include "ReflectoryViewBase.ixx"
#include "ReflectoryBase.hxx"
#include "TimeSpec.hxx"

namespace dueca {
template class ReflectoryViewBase<dueca::TimeTickType>;

} // namespace dueca
