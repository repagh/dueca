/* ------------------------------------------------------------------   */
/*      item            : TransportClass.hxx
        made by         : Rene' van Paassen
        date            : 001024
        category        : header file
        description     :
        changes         : 001024 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
        documentation   : DUECA_API
*/

#ifndef TransportClass_hh
#define TransportClass_hh

#include <dueca_ns.h>
#include <ChannelDef.hxx>

namespace dueca {

/** \file TransportClass.hxx
    This file defines the TransportClass enumerated type. */

/** Re-typed TransportClass for compatibility with old code */
typedef Channel::TransportClass TransportClass;
static const Channel::TransportClass Bulk = Channel::Bulk;
static const Channel::TransportClass Regular = Channel::Regular;
static const Channel::TransportClass HighPriority = Channel::HighPriority;

} // namespace dueca
#endif
