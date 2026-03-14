/* ------------------------------------------------------------------   */
/*      item            : InterpTable1.hxx
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

/** Interpolation data holder, for 1d interpolation tables. Either
    reads data from file, or uses a static array initialisation. */
template <class T, class I> class InterpTable1
{
  /** Node values of this first index. */
  const I index1;

  /** Table data, as one flat array. Stored in a row-major order, i.e.
      iterating fastest over the last dimension. Of course, for a 1 d
      array this does not really matter! */
  const T *data;

public:
  /** Constructor with an array pointer as input. */
  InterpTable1(const I &i1, const T *data) :
    index1(i1),
    data(data)
  {}

public:
  /** Access the first index, normally for getting fraction/index
      values. */
  inline const I &getIndex1() const { return index1; }

  /** Get node values */
  inline const T &getValue(int i1) const
  {
    if (i1 < 0)
      i1 = 0;
    if (i1 > index1.maxDim())
      i1 = index1.maxDim();
    return data[i1];
  }
};
DUECA_NS_END
