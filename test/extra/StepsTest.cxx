/* ------------------------------------------------------------------   */
/*      item            : StepsTest.cxx
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

#include "Steps.hxx"
#include <cassert>
#include <array>

#include <iostream>
using namespace dueca;

static double ui[] = {0.0, 0.5, 2.0, 3.0};
static double yi[] = {1.0, 2.0, 3.0, 4.0};

static double ui2[] = {1.0, 0.0, 0.5, 2.0, 3.0};
static double yi2[] = {2.5, 1.0, 2.0, 3.0, 4.0};

constexpr std::array<double, 4> uia{0.0, 0.5, 2.0, 3.0};
constexpr std::array<double, 4> yia{1.0, 2.0, 3.0, 4.0};

int main()
{
  try {
    // too low number
    Steps<1> s1(ui, yi);
    std::cerr << "accepting a 1 step step" << std::endl;
  }
  catch (const steps_exception& e) {
    // continue
  }

  Steps<4> s4(yi, ui);
  assert(s4(-1.0) == 1.0);
  assert(s4(0.5) == 2.0);
  assert(s4(0.3) == 2.0);
  assert(s4(4.0) == 4.0);

  Steps<4> s4a(yia, uia);
  assert(s4a(-1.0) == 1.0);
  assert(s4a(0.5) == 2.0);
  assert(s4a(0.3) == 2.0);
  assert(s4a(4.0) == 4.0);

  Steps<5> s5(yi2, ui2);
  assert(s5(0.9) == 2.5);
  assert(s5(-1.0) == 1.0);
  assert(s5(0.5) == 2.0);
  assert(s5(0.3) == 2.0);
  assert(s5(4.0) == 4.0);

  return 0;
}
