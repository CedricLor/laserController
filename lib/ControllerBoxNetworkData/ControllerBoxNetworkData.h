/*
  ControllerBoxNetworkData.h - Library to handle the ControlerBox network data attributes.
  Created by Cedric Lor, October 26, 2019.
*/
#ifndef ControllerBoxNetworkData_h
#define ControllerBoxNetworkData_h

#include "Arduino.h"
#include <globalBasementVars.h>

class ControllerBoxNetworkData
{
  friend class ControlerBox;

  public:
    /** -------- constructors -------- */
    ControllerBoxNetworkData();


    uint32_t nodeId;
    /** The stationIP may be set automatically (by an AP to which this box connects)
     * or manually (see myMeshStarter, etc.). */
    IPAddress stationIP;
    /** The APIP may set be set by painlessMesh, calculated from the ESP's mac address or set manually
     * when the IF is broadcasted on this ESP's AP interface. */
    IPAddress APIP;

  private:
    /** -------- Public Instance Methods -----------------
     * */
    void update(uint32_t _ui32SenderNodeId, JsonObject& _obj);
    const bool setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxDefaultState(const short _sBoxDefaultState);
    void setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime);

};

#endif
