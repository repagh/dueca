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
template <typename FN>

void check(const char* name, const FN& s, double u, double y)
{
  auto y2 = s(u);
  std::cout << name << "(" << u << ") = " << y2 << " ? " << y << std::endl;
  assert(std::abs(y-y2) < 1e-8);
}

int main()
{
  PolynomialN s1(2, ai);

  check("s1", s1, -1.0, 1.5);
  check("s1", s1, 0.0, 1.0);
  check("s1", s1, 1.0, 2.5);

  PolynomialN s2(aia);
  check("s2", s2, -1.0, 1.5);
  check("s2", s2, 0.0, 1.0);
  check("s2", s2, 1.0, 2.5);

  return 0;
}
