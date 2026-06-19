/* ------------------------------------------------------------------   */
/*      item            : IncoSpecExtra.cxx
        made by         : Rene' van Paassen
        date            : 130104
        category        : additional body code
        description     :
        changes         : 130104 first version
        language        : C++
        copyright       : (c) 2013 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

} // namespace dueca
#include <IncoTable.hxx>
namespace dueca {

#define __CUSTOM_COMPATLEVEL_110
void IncoSpec::setTable(const IncoTable* itabl)
{
  while (itabl->incovar != NULL) {
    table.push_back(*(itabl->incovar));
    itabl++;
  }
}
