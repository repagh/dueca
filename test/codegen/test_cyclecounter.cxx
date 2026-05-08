/* ------------------------------------------------------------------   */
/*      item            : test_CycleCounter.cxx
        made by         : Rene' van Paassen
        date            : 260508
        category        : body file
        description     : test the cyclec counter basics
        changes         : 260508 first version
        language        : C++
        copyright       : (c) 2026 René van Paassen
        license         : EUPL-1.2
*/

#include <cassert>

#define DUECA_CONFIG_MSGPACK
#include <udpcom/CycleCounter.hxx>

USING_DUECA_NS;

int main()
{
  CycleCounter c1;
  CycleCounter c2 = c1;
  CycleCounter cnext = c1.cycleIncrement();
  assert(cnext.cycleIsCurrent(cnext));
  assert(cnext.cycleIsCurrentOrPast(cnext));
  assert(cnext.cycleIsPrevious(c1));
  cnext.cycleRepeatIncrement();
  assert(cnext.cycleCount() == c1.cycleCount()+1);

  return 0;
}
