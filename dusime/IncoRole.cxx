/* ------------------------------------------------------------------   */
/*      item            : IncoRole.cxx
        made by         : Rene' van Paassen
        date            : 001009
        category        : body file
        description     :
        changes         : 001009 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/


#define IncoRole_cxx
#include "IncoRole.hxx"
#include "AmorphStore.hxx"
#include <iostream>
namespace dueca {

const char* const getString(const IncoRole &o)
{
  static const char* names[] = {
    "Control",
    "Target",
    "Constraint",
    "Unspecified"};
  return names[int(o)];
}

template<>
const char* getclassname<IncoRole>() {return "IncoRole";}

} // namespace dueca

namespace std {
ostream& operator << (ostream& s, const dueca ::IncoRole& o)
{
  return s << dueca::getString(o);
}

istream& operator >> (istream& is, dueca::IncoRole& o)
{
  std::string tmp; is >> tmp;
  for (o = dueca::Control; true; o = dueca::IncoRole(int(o)+1)) {
    if (o == dueca::NoIncoRoles || tmp == string(getString(o)) ) {
      return is;
    }
  }
}

} // namespace std

void packData(dueca::AmorphStore& s, const dueca::IncoRole& o)
{
  packData(s, uint8_t(o));
}

void unPackData(dueca::AmorphReStore& s, dueca::IncoRole &o)
{
  uint8_t tmp(s);
  o = dueca::IncoRole(tmp);
}
