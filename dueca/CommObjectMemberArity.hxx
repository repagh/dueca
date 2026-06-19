/* ------------------------------------------------------------------   */
/*      item            : CommObjectMemberArity.hxx
        made by         : Rene van Paassen
        date            : 141202
        category        : header file
        description     :
        api             : DUECA_API
        changes         : 141202 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#ifndef CommObjectMemberArity_hxx
#define CommObjectMemberArity_hxx
#include <dueca_ns.h>

namespace dueca {

/** Enumeration value defining the different possibilities for members. */
enum MemberArity {
  Single,         /**< The member is a single value */
  Iterable,       /**< The member is iterable, no key */
  Mapped,         /**< The member is iterable, with key */
  FixedIterable   /**< The member is iterable, but has fixed size, and
                       cannot use push_back */
};

} // namespace dueca

#endif
