/*
  ControlerBox.h - Library to replace box_type struct - handles the ControlerBox attributes.
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef ControlerBox_h
#define ControlerBox_h

#include "Arduino.h"

class ControlerBox
{
  public:
    ControlerBox();
    uint32_t nodeId; // calculated by painlessMesh from th mac address
    bool isNewBoxHasBeenSignaled;   // when it is a new box, mark it so that the interface can send a WS message
    IPAddress stationIP; // set by painlessMesh or, for the interface, by the AP to which it is connected
    IPAddress APIP; // set by painlessMesh
    byte bNodeName; // set by me, in global (201, 202, etc.)
    short int boxActiveState; // set by events or user
    bool boxActiveStateHasBeenSignaled; // set by events or user; usefull mainly on the interface, for the moment; when the mesh registers a boxState change, the WebSocket can read this and send a message to the browser
    bool boxActiveStateHasBeenTakenIntoAccount; // set to false when updating another box value (not in use for self) and to true by boxState, once boxState has taken the new state into account
    uint32_t uiBoxActiveStateStartTime; // written upon occurence of an event
    bool boxDeletionHasBeenSignaled; // set by events or user; usefull mainly on the interface, for the moment; when the mesh registers a box disconnection, the WebSocket can read this and send a message to the browser
    byte bMasterBoxName;
    bool bMasterBoxNameChangeHasBeenSignaled;
    short int sBoxDefaultState;
    bool sBoxDefaultStateChangeHasBeenSignaled;

    // void updateProperties();
    void updateThisBoxProperties();
    void printProperties(const uint8_t _ui8BoxIndex);
    void updateMasterBoxName(const byte _bMasterBoxName);

    // Signal catchers -- static variables
    static bool valFromPir;
    static uint32_t uiSettingTimeOfValFromPir;
    static short int valFromWeb;

    static int8_t findByNodeId(uint32_t _ui32nodeId);

    static void updateConnectedBoxCount(short int _newConnectedBoxesCount);
    static short int connectedBoxesCount;
    static short int previousConnectedBoxesCount;

    static void updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj);
    static void setBoxActiveState(const uint8_t _ui8BoxIndex, const short _sBoxActiveState, const uint32_t _uiBoxActiveStateStartTime);
    static void setBoxDefaultState(const uint8_t _ui8BoxIndex, const short _sBoxDefaultState);

    static void deleteBox(uint32_t _ui32nodeId);

    static Task tReboot;
    static void _reboot();

  private:
    static IPAddress _parseIpStringToIPAddress(JsonDocument& root, const char* rootKey/*String& rootKey*/);
    static void _parseCharArrayToBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
};

extern ControlerBox ControlerBoxes[];

#endif
