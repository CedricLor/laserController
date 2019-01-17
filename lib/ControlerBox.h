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

    static void boxTypeSelfUpdate(ControlerBox *ControlerBoxes, const short I_NODE_NAME, const short BOXES_I_PREFIX);
    // static void boxTypeUpdate(ControlerBox *ControlerBoxes, uint32_t iSenderNodeName, uint32_t senderNodeId, JsonObject& root, const short I_NODE_NAME, const short BOXES_I_PREFIX);
  private:
    static IPAddress parseIpString(JsonObject& root, String rootKey);
    static void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
};

#endif
