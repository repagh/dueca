/* ------------------------------------------------------------------   */
/*      item            : IncoRole.hxx
        made by         : Rene' van Paassen
        date            : 001009
        category        : header file
        description     :
        changes         : 001009 first version
        documentation   : DUECA_API
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef IncoRole_hxx
#define IncoRole_hxx

#include <iostream>
#include <inttypes.h>
#include <dueca_ns.h>
#include <CommObjectTraits.hxx>

namespace dueca {

class AmorphStore;
class AmorphReStore;

/** IncoRole defines the type of inco calculation.

    Note that the IncoRole definition given here is fairly
    haphazard. If you feel that a more elaborated IncoRole is needed,
    please contact me */

enum IncoRole {
  Control,    /**< The variable can be manipulated to obtain a stable trim
                     condition. */
  Target,     /**< The variable defines the stable trim condition. */
  Constraint, /**< The variable specifies a constraint for the trim
                   condition, e.g. gear down */
  Unspecified,/**< For a certain trim condition, this variable is not
                   a participant. */
  NoIncoRoles ///< not a real role, simply a counter
};

/// returns a string description of an IncoRole
const char* const getString(const IncoRole &o);

template<>
const char* getclassname<IncoRole>();

} // namespace dueca

/// print the IncoRole to a stream
namespace std {
/** print a string representation for inco role */
ostream& operator << (ostream& s, const dueca::IncoRole& o);
/** read a string representation for inco role */
istream& operator >> (istream& is, dueca::IncoRole& o);
} // namespace std

/** Support store packing */
void packData(dueca::AmorphStore& s, const dueca::IncoRole &o);
/** Support store unpacking */
void unPackData(dueca::AmorphReStore& s, dueca::IncoRole &o);

#endif
