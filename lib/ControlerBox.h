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
    IPAddress stationIP; // set by painlessMesh or, for the interface, by the AP to which it is connected
    IPAddress APIP; // set by painlessMesh
    byte bNodeName; // set by me, in global (201, 202, etc.)
    short int boxActiveState; // set by events or user
    bool boxActiveStateHasChanged; // set by events or user
    uint32_t uiBoxActiveStateStartTime; // written upon occurence of an event

    // void updateProperties();
    void updateThisBoxProperties();
    void printProperties(const byte bBoxIndex);

    // Signal catchers -- static variables
    static bool valFromPir;
    static uint32_t uiSettingTimeOfValFromPir;
    static short int valFromMesh;
    static uint32_t uiSettingTimeOfValFromMesh;
    static short int valFromWeb;

    static short int connectedBoxesCount;

    static void updateOtherBoxProperties(uint32_t senderNodeId, JsonDocument& doc);
    static void setBoxActiveState(const byte bBoxIndex, JsonDocument& doc);

  private:
    static IPAddress _parseIpStringToIPAddress(JsonDocument& root, const char* rootKey/*String& rootKey*/);
    static void _parseCharArrayToBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
};

#endif
