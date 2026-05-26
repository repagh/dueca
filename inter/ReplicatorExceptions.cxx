/* ------------------------------------------------------------------   */
/*      item            : ReplicatorExceptions.cxx
        made by         : Rene' van Paassen
        date            : 170217
        category        : body file
        description     :
        changes         : 170217 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define ReplicatorExceptions_cxx
#include "ReplicatorExceptions.hxx"

namespace dueca {

const char* dataclassdiffers::what() const throw()
{
  return "Data type differs across replicated nodes, check dco file";
}

dataclassdiffers::dataclassdiffers():
  std::exception()
{
  //
}

} // namespace dueca
