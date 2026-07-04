/* ------------------------------------------------------------------   */
/*      item            : PythonScripting.hxx
        made by         : Rene van Paassen
        date            : 180220
        category        : header file
        description     :
        changes         : 180220 first version
        language        : C++
        copyright       : (c) 2018 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef PythonScripting_hxx
#define PythonScripting_hxx

#include <dueca/ScriptHelper.hxx>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#undef HAVE_PTHREAD_H
#undef HAVE_SCHED_H
#undef HAVE_SETUID
#undef HAVE_SIGNAL_H
#undef HAVE_STDLIB_H
#undef HAVE_SYS_PARAM_H
#undef HAVE_SYS_TIME_H
#undef HAVE_UNISTD_H
#undef HAVE_FCNTL_H
#include <fstream>
namespace bpy = boost::python;

#include <string>
#include <dueca_ns.h>

namespace dueca {


/** Implements the interface to Python */
struct PythonScripting : public ScriptHelper
{
  /** scratch file */
  std::ofstream scratchfile;

  /** main module? */
  bpy::object main_module;

  /** main namespace for the script */
  bpy::object main_namespace;

  /** lock state */
  PyGILState_STATE _state;

  /** continue flag */
  bool running;

  /** Constructor */
  PythonScripting();

  /** Destructor */
  ~PythonScripting();

  /** Perform preliminary initialisation */
  void initiate() final;

  /** Start the interpreter */
  void interpreter() final;

  /** Read a single line from the module script file */
  bool readline(std::string &line) final;

  /** Write a single line to a scratch file */
  bool writeline(const std::string &line) final;

  /** Run a single string of code directly */
  void runCode(const char *code) final;

  /** Lock the scripting access */
  bool acquireScriptingLock() final;

  /** Release the scripting access */
  void releaseScriptingLock() final;
};

} // namespace dueca
#endif
