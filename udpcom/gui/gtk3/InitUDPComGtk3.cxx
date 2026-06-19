/* ------------------------------------------------------------------   */
/*      item            : InitDusimeGtk.cxx
        made by         : Rene' van Paassen
        date            : 021001
        category        : body file
        description     :
        changes         : 021001 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
*/

#include <dueca/visibility.h>
#include <StartIOStream.hxx>
#include <iostream>
#include "NetUseOverviewGtk3.hxx"
#define DO_INSTANTIATE
#include "TypeCreator.hxx"
#include <dueca_ns.h>

using namespace dueca;

extern "C"
LNK_PUBLICC void InitUDPComGtk3()
{
  startIOStream();
  if (!DuecaEnv::scriptSpecific()) {
    std::cout << "Init from     [dueca-udp-gtk3]" << std::endl;
  }
  static TypeCreator<NetUseOverviewGtk3>
    t03(NetUseOverview::getMyParameterTable());
}


