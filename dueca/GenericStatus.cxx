/* ------------------------------------------------------------------   */
/*      item            : GenericStatus.cxx
        made by         : Rene' van Paassen
        date            : 010822
        category        : body file
        description     :
        changes         : 010822 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/


#define GenericStatus_cxx
#include "GenericStatus.hxx"
namespace dueca {

GenericStatus::GenericStatus()
{
  //
}

GenericStatus::~GenericStatus()
{
  //
}
/*
  GenericStatus GenericStatus::operator && (GenericStatus& o) const; */

GenericStatus& GenericStatus::operator &= (GenericStatus& o)
{
  operatorAndEq(o);
  return *this;
}

} // namespace dueca
