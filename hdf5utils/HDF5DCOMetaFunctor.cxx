/* ------------------------------------------------------------------   */
/*      item            : HDF5DCOMetaFunctor.cxx
        made by         : Rene' van Paassen
        date            : 170327
        category        : body file
        description     :
        changes         : 170327 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define HDF5DCOMetaFunctor_cxx
#include "HDF5DCOMetaFunctor.hxx"

namespace dueca { namespace hdf5log {

HDF5DCOMetaFunctor::HDF5DCOMetaFunctor()
{

}


HDF5DCOMetaFunctor::~HDF5DCOMetaFunctor()
{

}

const H5::DataType* HDF5DCOMetaFunctor::operator() ()
{
  return NULL;
}

} } // namespace dueca namespace hdf5log
