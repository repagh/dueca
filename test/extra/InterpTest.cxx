/* ------------------------------------------------------------------   */
/*      item            : InterpTest.cxx
        made by         : Rene' van Paassen
        date            : 010614
        category        : body file
        description     :
        changes         : 010614 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define InterpTest_cxx

#include "InterpIndex.hxx"
#include "InterpTable3.hxx"
#include "Interpolator3.hxx"
#include <cmath>

#include <iostream>
using namespace dueca;
// table with dimensions (2, 4, 3)
static double data[] = { 0.0,  3.4,  3.6,   // row 1, layer 1
                         1.2,  2.2,  3.2,   // row 2, layer 1
                         2.4,  2.2,  2.8,   // row 3, layer 1
                         2.5,  1.5,  1.6,   // row 4, layer 1
                         10.0, 13.4, 13.6,   // row 1, layer 2
                         11.2, 12.2, 13.2,   // row 2, layer 2
                         12.4, 12.2, 12.8,   // row 3, layer 2
                         12.5, 11.5, 11.6 };  // row 4, layer 2

static double i1[] = { 0.0, 2.0 };
static double i2[] = { 1.2, 3.4, 3.8, 5.6 };
static double i3[] = { 0.0, 3.0, 5.0 };

int main()
{
  // interpolation indices
  InterpIndex<double> index1(2, i1);
  InterpIndex<double> index2(4, i2);
  InterpIndex<double> index3(3, i3);

  // The table we will use
  InterpTable3<double, InterpIndex<double>> table(index1, index2, index3, data);

  // The objects used to iterate over the table. They shoudlbe
  // independent
  Interpolator3<double> it1(index1, index2, index3);
  Interpolator3<double> it2(index1, index2, index3);

  // try something. Border cases
  it1.updateIndices(0.0, 1.2, 0.0);
  it2.updateIndices(2.0, 5.6, 5.0);
  std::cout << "values_1 " << it1.getValue(table) << ' ' << it2.getValue(table)
            << std::endl;
  if (it1.getValue(table) != 0.0 || it2.getValue(table) != 11.6)
    return 1;

  // something else, somewhere in between
  it1.updateIndices(1.0, 1.8, 2.0);
  it2.updateIndices(1.99, 3.799, 3.001);
  std::cout << "values_2 " << it1.getValue(table) << ' ' << it2.getValue(table)
            << std::endl;
  if (std::fabs(it1.getValue(table) - 7.15) > 0.01 ||
      std::fabs(it2.getValue(table) - 12.15) > 0.01)
    return 2;

  // something else, outside table (should keep to limit)
  it1.updateIndices(3.0, 1.8, 2.0);
  it2.updateIndices(1.99, 0.0, 3.001);
  std::cout << "values_3 " << it1.getValue(table) << ' ' << it2.getValue(table)
            << std::endl;
  if (std::fabs(it1.getValue(table) - 12.16) > 0.01 ||
      std::fabs(it2.getValue(table) - 13.35) > 0.01)
    return 3;

  it1.updateIndices(2.0, 3.4, 3.0);
  it2.updateIndices(2.0, 5.7, 5.0);
  std::cout << "values_4 " << it1.getValue(table) << ' ' << it2.getValue(table)
            << std::endl;

  if (std::fabs(it1.getValue(table) - 12.2) > 0.01 ||
      std::fabs(it2.getValue(table) - 11.6) > 0.01)
    return 4;
}
