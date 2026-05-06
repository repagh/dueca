/* ------------------------------------------------------------------   */
/*      item            : msgpack4.cxx
        made by         : Rene' van Paassen
        date            : 260506
        category        : body file
        description     :
        changes         : 260506 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

// Test for just the msgpack streaming and unstreaming

#define msgpack4_cxx
#include <cassert>

#define MSGPACK_USE_DEFINE_MAP

#include <map>
#include <dueca/smartstring.hxx>
#include <dueca/MessageBuffer.hxx>
#include <dueca/fixvector.hxx>
#include <dueca/varvector.hxx>
#include <cstring>
#include <dueca/msgpack.hxx>

// last, to include code for the implementations
#include <dueca/msgpack-unstream-iter.ixx>

#define TEST_STRINGS 1
#define TEST_ARRAYS 1
#define TEST_MAPS 1

USING_DUECA_NS;

int main()
{
#if TEST_STRINGS
  {
  // strings of various sizes
    const char *s0 = "short string";
    std::string s1("more than 31 characters long must this one be!");
    assert(s1.size() > 31);
    std::string s2("More than 255 characters long this must be " + s1 + s1 +
                   s1 + s1 + s1);
    assert(s2.size() > 255);
    std::string s3("More than 2^16-1 characters");
    for (auto i = 250U; i--;) {
      s3 += s2;
    }
    assert(s3.size() > 256 * 256 - 1);

    dueca::MessageBuffer buf(120000);

    msgpack::packer<dueca::MessageBuffer> pk(buf);
    pk.pack(s0);
    pk.pack(s1);
    pk.pack(s2);
    pk.pack(s3);

    // streaming unpack
    auto i0 = buf.begin();
    auto iend = buf.end();

    std::string s0b, s1b, s2b, s3b, s4b;

    msgunpack::msg_unpack(i0, iend, s0b);
    assert(s0b.size() == std::strlen(s0));

    msgunpack::msg_unpack(i0, iend, s1b);
    assert(s1 == s1b);

    msgunpack::msg_unpack(i0, iend, s2b);
    assert(s2 == s2b);

    msgunpack::msg_unpack(i0, iend, s3b);
    assert(s3 == s3b);

    // release, because there is implicitly one user
    buf.release();
  }
#endif

#if TEST_ARRAYS
  {
    dueca::fixvector<5, int32_t> v1(1);

    dueca::fixvector<40, uint16_t> v2(2);

    dueca::fixvector<300, uint16_t> v3(3);

    dueca::fixvector<66000, uint8_t> v4(4);

    dueca::MessageBuffer buf(900000);

    msgpack::packer<dueca::MessageBuffer> pk(buf);

    pk.pack(v1);
    pk.pack(v2);
    pk.pack(v3);
    pk.pack(v4);

    // streaming unpack
    auto i0 = buf.begin();
    auto iend = buf.end();

    dueca::varvector<int16_t> c1;
    msgunpack::msg_unpack(i0, iend, c1);
    assert(c1.size() == v1.size());

#if 0
    try {
      dueca::fixvector<41, uint32_t> c2fail;
      msgunpack::msg_unpack(i0, iend, c2fail);
      assert(0 && "unpacking fix of 41 should fail");
    }
    catch (const std::exception &e) {
      std::cout << "as expected " << e.what() << std::endl;
    }
#endif
    dueca::fixvector<40, uint32_t> c2;
    msgunpack::msg_unpack(i0, iend, c2);
    assert(c2[0] == v2[0]);
    assert(c2.size() == v2.size());

    dueca::fixvector<300, uint16_t> c3;
    msgunpack::msg_unpack(i0, iend, c3);
    assert(c3 == v3);

    dueca::fixvector<66000, uint8_t> c4;
    msgunpack::msg_unpack(i0, iend, c4);
    assert(v4 == c4);
    // release, because there is implicitly one user
    buf.release();
  }

#endif

#if TEST_MAPS
  {
    std::map<int, int> v1;
    for (int i = 8; i--;) {
      v1[i] = i;
    }

    dueca::MessageBuffer buf(900000);

    msgpack::packer<dueca::MessageBuffer> pk(buf);

    pk.pack(v1);

    for (int i = 80; i--;) {
      v1[i] = i;
    }
    pk.pack(v1);

    for (int i = 800; i--;) {
      v1[i] = i;
    }
    pk.pack(v1);

    for (int i = 80000; i--;) {
      v1[i] = i;
    }
    pk.pack(v1);

    // streaming unpack
    auto i0 = buf.begin();
    auto iend = buf.end();

    std::map<int, int> c1, c2, c3, c4;

    msgunpack::msg_unpack(i0, iend, c1);
    assert(c1.size() == 8);

    msgunpack::msg_unpack(i0, iend, c2);
    assert(c2.size() == 80);

    msgunpack::msg_unpack(i0, iend, c3);
    assert(c3.size() == 800);

    msgunpack::msg_unpack(i0, iend, c4);
    assert(c4.size() == 80000);

    // release, because there is implicitly one user
    buf.release();
  }

#endif

  return 0;
}
