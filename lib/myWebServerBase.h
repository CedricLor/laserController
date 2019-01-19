/*
  myWebServerBase.h - Library web server controller related functions.
  Created by Cedric Lor, January 19, 2019.
*/

#ifndef myWebServerBase_h
#define myWebServerBase_h

#include "Arduino.h"

class myWebServerBase
{
  public:
    myWebServerBase(/*int pin*/);

    static void startAsyncServer();

  private:
    // int _pin;
    static AsyncWebServer asyncServer;
    static char linebuf[];
    static short charcount;

    static void listAllCollectedHeaders(AsyncWebServerRequest *request);
    static void listAllCollectedParams(AsyncWebServerRequest *request);

    static void onRequest(AsyncWebServerRequest *request);
    static void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
