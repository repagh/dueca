/* ------------------------------------------------------------------   */
/*      item            : dueca_ns.h
        made by         : Rene' van Paassen
        date            : 20010308
        category        : header file (in)
        description     : Defines the choice for namespace.
        changes         : 161124 RvP; make dueca always use namespace
        documentation   : in DUECA_API, but directly from configure
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

/** \file dueca_ns.h
    This file defines the namespace DUECA for dueca
    and dusime. It defines the following macros:

    <ul>
    <li>DUECA_NS_ON. If defined, this DUECA version uses namespace
    <li>DUECA_NS_START. Used in DUECA code to indicate the start of
    the namespace. Defined as "namespace dueca {".
    <li>DUECA_NS_END. Used in DUECA code to indicate the end of the
    namespace. Defined as "}".
    <li>DUECA_NS. The name of the DUECA namespace, "dueca"
    <li>CCDUECA_NS. Absolute DUECA namespace, "::dueca"
    <li>USING_DUECA_NS. Defined as "using namespace dueca;"
    <li>MSGPACKUS_NS_START. The namespace version for unpacking from
    msgpack. Defined as "namespace msgunpack {".
    <li>MSGPACKUS_NS_END. Defined as "}".
    <li>CCMSGPACKUS_NS. Absolute unpack namespace, "::msgunpack"
    </ul>
*/
#pragma once

namespace dueca {}

#define USING_DUECA_NS using namespace dueca;