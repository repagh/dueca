/* ------------------------------------------------------------------   */
/*      item            : TriggerAtom.hxx
        made by         : Rene van Paassen
        date            : 141111
        category        : header file
        description     :
        changes         : 141111 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef TriggerAtom_hxx
#define TriggerAtom_hxx

#include "TimeSpec.hxx"
#include "dueca_ns.h"

namespace dueca {

class TriggerTarget;

class TriggerAtom
{
public:
  /** Initial recipient for the triggering */
  TriggerTarget           *target;

  /** Time associated with this triggering */
  DataTimeSpec             ts;

  /** ID of the triggering puller */
  unsigned                 id;

  /** Constructor */
  TriggerAtom();

  /** Destructor */
  ~TriggerAtom();

  /** propagate the action */
  void propagate() const;
};

} // namespace dueca

#endif
