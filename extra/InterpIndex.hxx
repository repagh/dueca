/* ------------------------------------------------------------------   */
/*      item            : InterpIndex.hxx
        made by         : Rene van Paassen
        date            : 010614
        category        : header file
        description     :
        changes         : 010614 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
        documentation   : DUECA_API
*/

#pragma once

#include <dueca_ns.h>

DUECA_NS_START

/** This implements a single index (axis) for an interpolation into a
    table. */
template <class T> class InterpIndex
{
  /** number of elements in the index. */
  const int n;

  /** maximum element, n - 1, as a separate variable for speed. */
  const int n_m1;

  /** maximum element, n - 2, as a separate variable for speed. */
  const int n_m2;

  /** Index with points. */
  const T *ipts;

public:
  /** Constructor from static data. */
  InterpIndex(int n, const T *ipts) :
    n(n),
    n_m1(n - 1),
    n_m2(n - 2),
    ipts(ipts)
  {}

public:
  /** return the maximum allowed value for an index */
  inline int maxDim() const { return n_m1; }

  /** return the number of indexes. */
  inline int nDim() const { return n; }

  /** update an index, and fraction, return whether within the table. */
  inline bool updateIndex(int &index, double &frac, const T &value) const
  {
    if (index < 0)
      index = 0;
    if (index > n_m2)
      index = n_m2;
    while (index > 0 && value < ipts[index])
      index--;
    while (index < n_m2 && value > ipts[index + 1])
      index++;
    frac = (value - ipts[index]) / (ipts[index + 1] - ipts[index]);
    return (frac >= 0.0 && frac <= 1.0);
  }
};
DUECA_NS_END
