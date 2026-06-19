/* ------------------------------------------------------------------   */
/*      item            : ReferenceHolderPython.hxx
        made by         : Rene van Paassen
        date            : 251016
        category        : header file
        description     :
        changes         : 251016 first version
        language        : C++
        copyright       : (c) 2025 René van Paassen
*/

#pragma once

#if defined(SCRIPT_PYTHON)
#include "ReferenceHolder.hxx"
#include <boost/scoped_ptr.hpp>
#include <boost/python.hpp>

namespace bpy = boost::python;
namespace dueca {

struct ReferenceHolderPython : ReferenceHolder
{
  // bpy::list refs;
  boost::scoped_ptr<bpy::list> _refs;
  ReferenceHolderPython();

  bpy::list &refs();

  ~ReferenceHolderPython() override;
};

} // namespace dueca

#endif
