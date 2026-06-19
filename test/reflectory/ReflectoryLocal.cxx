/* ------------------------------------------------------------------   */
/*      item            : ReflectoryLocal.cxx
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

#define ReflectoryLocal_cxx
#include "ReflectoryLocal.ixx"
#include "ReflectoryViewBase.hxx"
#include "TimeSpec.hxx"

namespace dueca {

// template specialization
template class ReflectoryLocal<dueca::TimeTickType>;


} // namespace dueca
