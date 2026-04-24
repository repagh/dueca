/* ------------------------------------------------------------------   */
/*      item            : CircularWithPolyTest.cxx
        made by         : Rene' van Paassen
        date            : 260424
        category        : body file
        description     :
        changes         : 260424 first version
        language        : C++
        copyright       : (c) 2026 René van Paassen
        license         : EUPL-1.2
*/

#define InterpTest_cxx

#include <extra/CircularWithPoly.hxx>
#include <cassert>
#include <array>

using namespace dueca;

static double ai[] = { 1.0, 0.5, 1.0 };

constexpr std::array<double, 4> aia{ 1.0, 0.5, 1.0 };

int main()
{
  CircularWithPoly s1(1.0/100.0, 20.0, 0.0, 2, ai);

  assert(s1(-70) == 1.0);
  assert(s1(20.0) == 1.0);
  assert(s1(70.0) == 1.5);

  CircularWithPoly s2(1.0/100.0, 20.0, 0.0, aia);
  assert(s1(-70.0) == 1.0);
  assert(s1(20.0) == 1.0);
  assert(s1(70.0) == 1.5);

  return 0;
}
