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
#include <extra/Circular.hxx>
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
  //assert(std::abs(y-y2) < 1e-8);
}

int main()
{
  CircularWithPoly s1(1.0/100.0, 20.0, 0.0, 2, ai);
  Circular c1(1.0/100.0, 20.0, 1.0, 0.0);
  PolynomialN p1(2, ai);

  check("s1", s1, -30, p1(c1(-30)));
  check("s1", s1, -5, p1(c1(-5)));
  check("s1", s1, 20, p1(c1(20)));
  check("s1", s1, 70, p1(c1(70)));


  CircularWithPoly s2(1.0/100.0, 20.0, -0.5, aia);
  Circular c2(1.0/100.0, 20.0, 1.0, -0.5);
  PolynomialN p2(aia);

  check("s2", s2, -30, p2(c2(-30)));
  check("s2", s2, -5, p2(c2(-5)));
  check("s2", s2, 20, p2(c2(20)));
  check("s2", s2, 70, p2(c2(70)));

  return 0;
}
