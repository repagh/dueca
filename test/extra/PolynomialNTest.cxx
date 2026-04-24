/* ------------------------------------------------------------------   */
/*      item            : PolynomialNTest.cxx
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

#include <extra/PolynomialN.hxx>
#include <cassert>
#include <array>

using namespace dueca;

static double ai[] = { 1.0, 0.5, 1.0 };

constexpr std::array<double, 4> aia{ 1.0, 0.5, 1.0 };

int main()
{
  PolynomialN s1(2, ai);

  assert(s1(-1.0) == -0.5);
  assert(s1(0.0) == 1.0);
  assert(s1(1.0) == 2.5);

  PolynomialN s2(aia);
  assert(s1(-1.0) == -0.5);
  assert(s1(0.0) == 1.0);
  assert(s1(1.0) == 2.5);

  return 0;
}
