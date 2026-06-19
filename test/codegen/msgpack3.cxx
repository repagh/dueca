/* ------------------------------------------------------------------   */
/*      item            : msgpack2.cxx
        made by         : Rene' van Paassen
        date            : 121230
        category        : body file
        description     :
        changes         : 121230 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define msgpack2_cxx
#include <cassert>

#define MSGPACK_USE_DEFINE_ARRAY

#include <dueca/CommObjectWriter.hxx>
#include <AmorphStore.hxx>
#include <dueca/MessageBuffer.hxx>
#include <dueca/msgpack.hxx>
#include <dueca/msgpack-unstream-iter.ixx>
using namespace std;

using namespace dueca;

int main()
{
#if 1
  dueca::MessageBuffer buf(200);
  int ii = 1;
  msgpack::packer<dueca::MessageBuffer> pk(buf);
  pk.pack(ii);
  int i2 = 0;
  auto i0 = buf.begin();
  auto iend = buf.end();
  msgunpack::msg_unpack(i0, iend, i2);
  assert(ii == i2);
#endif

  return 0;
}
