/* ------------------------------------------------------------------   */
/*      item            : DDFFDCOWriteFunctor.cxx
        made by         : Rene' van Paassen
        date            : 211230
        category        : body file
        description     :
        changes         : 211230 first version
        language        : C++
        copyright       : (c) 21 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define DDFFDCOWriteFunctor_cxx
#include "DDFFDCOWriteFunctor.hxx"

namespace dueca { namespace ddff {

DDFFDCOWriteFunctor::
DDFFDCOWriteFunctor(bool rtick) :
  itend(),
  it_ptr(&itend),
  rtick(rtick)
{
  //
}


DDFFDCOWriteFunctor::~DDFFDCOWriteFunctor()
{
  //
}

void DDFFDCOWriteFunctor::setIterator(FileStreamRead::Iterator& it)
{
  this->it_ptr = &it;
}

} } // namespace ddff namespace dueca
