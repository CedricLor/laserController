/*
  controllerBoxNetworkData.h - Library to handle the ControlerBox network data attributes.
  Created by Cedric Lor, October 26, 2019.
*/
#ifndef controllerBoxNetworkData_h
#define controllerBoxNetworkData_h

#include "Arduino.h"
#include <globalBasementVars.h>

class controllerBoxNetworkData
{
  friend class ControlerBox;

  public:
    /** -------- constructors -------- */
    controllerBoxNetworkData();


    uint32_t nodeId;
    /** The stationIP may be set automatically (by an AP to which this box connects)
     * or manually (see myMeshStarter, etc.). */
    IPAddress stationIP;
    /** The APIP may set be set by painlessMesh, calculated from the ESP's mac address or set manually
     * when the IF is broadcasted on this ESP's AP interface. */
    IPAddress APIP;

    // void printProperties(const uint16_t _ui16BoxIndex);

  private:
    void update(uint32_t _ui32SenderNodeId, JsonObject& _obj);
};

#endif
