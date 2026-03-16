/* ------------------------------------------------------------------   */
/*      item            : InterpTable3.hxx
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

/** Interpolation data holder, for 3d interpolation tables. Either
    reads data from file, or uses a static array initialisation. */
template <class T, class I> class InterpTable3
{
  /** Node values of this first index. */
  const I index1;

  /** Node values of this second index. */
  const I index2;

  /** Node values of this third index. */
  const I index3;

  /** Table data, as one flat array. Stored in a row-major order, i.e.
      iterating fastest over the last dimension. */
  const T *data;

public:
  /** Constructor with an array pointer as input, by far the fastest
      constructor. */
  InterpTable3(const I &i1, const I &i2, const I &i3, const T *data) :
    index1(i1),
    index2(i2),
    index3(i3),
    data(data)
  {}

public:
  /** Access the first index, normally for getting fraction/index
      values. */
  inline const I &getIndex1() const { return index1; }

  /** Access the second index, normally for getting fraction/index
      values. */
  inline const I &getIndex2() const { return index2; }

  /** Access the third index, normally for getting fraction/index
      values. */
  inline const I &getIndex3() const { return index3; };

  /** Get node values */
  inline const T &getValue(int i1, int i2, int i3) const
  {
    if (i1 < 0)
      i1 = 0;
    if (i1 > index1.maxDim())
      i1 = index1.maxDim();
    if (i2 < 0)
      i2 = 0;
    if (i2 > index2.maxDim())
      i2 = index2.maxDim();
    if (i3 < 0)
      i3 = 0;
    if (i3 > index3.maxDim())
      i3 = index3.maxDim();
    return data[(i1 * index2.nDim() + i2) * index3.nDim() + i3];
  }
};
DUECA_NS_END
