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
    uint32_t nodeId;
    IPAddress stationIP;
    IPAddress APIP;
    short int iNodeName;

    void updateProperties();
    void updateThisBoxProperties();
    // static void boxTypeUpdate(ControlerBox *ControlerBoxes, uint32_t iSenderNodeName, uint32_t senderNodeId, JsonObject& root);

    static bool valPir;
    static bool valMesh;

  private:
    static IPAddress _parseIpString(JsonObject& root, String rootKey);
    static void _parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
};

#endif
