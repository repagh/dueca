/* ------------------------------------------------------------------   */
/*      item            : GuileStart.hh
        made by         : Rene' van Paassen
        date            : 990702
        category        : header file
        description     :
        changes         : 990702 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef GuileStart_hh
#define GuileStart_hh

#include <dueca_ns.h>
namespace dueca {

/** Templated function, needs to be instantiated */
template<class T>
void scheme_init();

/** Class for attachment of a DUECA object to Scheme. Creation of a
    single object of this class stands for creation of a call in
    Scheme to create the object and an object type to access the same
    from Scheme. */
template<class T>
class GuileStart
{
public:
  /** Constructor. */
  GuileStart();

  /** Destructor. */
  ~GuileStart();

  /** Initialisation function, indirectly called from Scheme. */
  static void schemeInit();
};

} // namespace dueca
#endif


//--------------------------------------------------------------------
// IMPLEMENTATION
//--------------------------------------------------------------------

#if defined(GuileStart_cxx) || defined(DO_INSTANTIATE)
#ifndef GuileStart_ii
#define GuileStart_ii

#ifdef SCRIPT_SCHEME

#include "ScriptInterpret.hxx"
#include "DuecaEnv.hxx"
#include <SchemeClassData.hxx>

#include <dueca_ns.h>
namespace dueca {
template<class T>
static void ifunct(void)
{
  //GuileStart<T>::schemeInit();
  scheme_init<T>();
}

template<class T>
GuileStart<T>::GuileStart()
{
  if (!DuecaEnv::scriptSpecific()) {
    std::cout << "adding object (" << SchemeClassData<T>::single()->getName() << ")" << std::endl;
  }
  ScriptInterpret::addInitFunction(SchemeClassData<T>::single()->getName(), NULL, ifunct<T>);
}

template<class T>
GuileStart<T>::~GuileStart()
{
  //
}

template<class T>
void GuileStart<T>::schemeInit()
{
  T::schemeInit();
}
} // namespace dueca
#endif
#endif
#endif
