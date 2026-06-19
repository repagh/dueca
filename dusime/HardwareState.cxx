/* ------------------------------------------------------------------   */
/*      item            : HardwareState.cxx
        made by         : Rene' van Paassen
        date            : 010226
        category        : body file
        description     :
        changes         : 010226 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/


#define HardwareState_cxx
#include "HardwareState.hxx"
namespace dueca {

static const char* HardwareState_names[] = {
  "Down",
  "Neutral",
  "Testing",
  "Active"
}
} // namespace dueca

namespace std {
ostream& operator << (ostream& os, const dueca::HardwareState& o)
{
  return os << HardwareState_names[int(o)];
}
} // namespace std
