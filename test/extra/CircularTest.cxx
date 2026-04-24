/* ------------------------------------------------------------------   */
/*      item            : CircularTest.cxx
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

#include <extra/Circular.hxx>
#include <cassert>
#include <array>
#include <iostream>
#include <cmath>

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
  Circular s1(1.0/100.0, 20.0, 360.0, -180.0);
  check("s1", s1, -30, -180);
  check("s1", s1, -5, -90);
  check("s1", s1, 20, 0);
  check("s1", s1, 70, 180);

  Circular s2(1.0/100.0, 20.0, 360.0, 0.0);

  check("s2", s2, -30, 0);
  check("s2", s2, -5, 90);
  check("s2", s2, 20, 180);
  check("s2", s2, 70, 360);

  return 0;
}
