/* ------------------------------------------------------------------   */
/*      item            : LogString.cxx
        made by         : Rene' van Paassen
        date            : 061128
        category        : body file
        description     :
        changes         : 061128 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define LogString_cxx
#include "LogString.hxx"
#include "Dstring.ixx"

namespace dueca {
template class Dstring<LSSIZE>;
} // namespace dueca

namespace std {
template std::ostream& operator << (std::ostream& os,
                                    const dueca ::Dstring<LSSIZE>& o);
} // namespace std

template void packData(dueca ::AmorphStore& s,
                       const dueca ::Dstring<LSSIZE>& o);
template void unPackData(dueca ::AmorphReStore& s,
                         dueca ::Dstring<LSSIZE>& o);

