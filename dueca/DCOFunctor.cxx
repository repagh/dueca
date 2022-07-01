/* ------------------------------------------------------------------   */
/*      item            : DCOFunctor.cxx
        made by         : Rene' van Paassen
        date            : 170326
        category        : body file
        description     :
        changes         : 170326 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#define DCOFunctor_cxx
#include "DCOFunctor.hxx"

DUECA_NS_START;

DCOFunctor::DCOFunctor() { }

DCOFunctor::~DCOFunctor() { }

bool DCOFunctor::operator() (const void* dpointer, const DataTimeSpec& ts)
{ return false; }
bool DCOFunctor::operator() (void* pointer)
{ return false; }

DUECA_NS_END;
