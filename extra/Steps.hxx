/* ------------------------------------------------------------------   */
/*      item            : Steps.hxx
        made by         : Rene van Paassen
        date            : 030228
        category        : header file
        description     :
        documentation   : DUECA_API
        changes         : 030228 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#pragma once

#include <dueca_ns.h>
#include <iostream>
#include <exception>

DUECA_NS_START

/** An auxiliary function for Steps. */
void Steps_sort_and_copy(const double *ui, const double *yi, double *tp, double *y, int n);
std::ostream &Steps_print(std::ostream& os, double *tp, double *y, int n);

class steps_exception : public std::exception
{
public:
  /** Re-implementation of std:exception what. */
  const char *what() const throw();
};


/** Implementation of a scaling/converting device, that produces a
    discrete number of different output values depending on a
    continous input. This can be used to e.g. implement IO signals for
    a flap handle. */
template <int n> class Steps
{
  /** Array of output values. */
  double y[n];

  /** Array of input/measured value transition points. */
  double tp[n - 1];

public:
  /** Constructor.
      \param yi    Set of possible output values for this device.
      \param ui    Input values corresponding to the output
                   values. */
  Steps(double yi[n], double ui[n]) { Steps_sort_and_copy(ui, yi, tp, y, n); }

  /** Templated constructor with array-like stuff */
  template <typename A> Steps(const A &yi, const A &ui)
  {
    if (yi.size() != n || ui.size() != n) {
      throw steps_exception();
    }
    Steps_sort_and_copy(ui.data(), yi.data(), tp, y, n);
  }

  /** Destructor. */
  ~Steps() {}

  /** The operator. */
  double operator()(const double x) const
  {
    for (int ii = n - 1; ii--;) {
      if (x > tp[ii])
        return y[ii + 1];
    }
    return y[0];
  }

  /** Print the object */
  std::ostream &print(std::ostream &os) const { return Steps_print(os, tp, y, n); }
};

DUECA_NS_END

template <int n>
inline std::ostream &operator<<(std::ostream &os, const DUECA_NS::Steps<n> &o)
{
  return o.print(os);
}
