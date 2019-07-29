/*
  mySpiffs.h - access to the spiffs
  Created by Cedric Lor, June 28, 2019.
*/

#ifndef mySpiffs_h
#define mySpiffs_h

#include "Arduino.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#define FORMAT_SPIFFS_IF_FAILED true

class mySpiffs
{
  public:
    mySpiffs();

    void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    // listDir(SPIFFS, "/", 0);
    void createDir(fs::FS &fs, const char * path);
    // void createDir(SPIFFS, "/mydir");
    void removeDir(fs::FS &fs, const char * path);
    // void removeDir(SPIFFS, "/mydir");

    void readFile(fs::FS &fs, const char * path);
    // readFile(SPIFFS, "/hello.txt")
    void readJSONObjLineInFile(const char * path, void (&callBack)(JsonObject&), uint16_t _ui16stepCounter);

    void readLine(File& file, uint16_t _ui16stepCounter, char* _cStep);

    void writeFile(fs::FS &fs, const char * path, const char * message);
    // writeFile(SPIFFS, "/hello.txt", "Hello ");
    void appendFile(fs::FS &fs, const char * path, const char * message);
    // appendFile(SPIFFS, "/hello.txt", "World!\n");
    void renameFile(fs::FS &fs, const char * path1, const char * path2);
    // renameFile(SPIFFS, "/hello.txt", "/world.txt");
    void deleteFile(fs::FS &fs, const char * path);
    // deleteFile(SPIFFS, "/hello.txt");
    void testFileIO(fs::FS &fs, const char * path);
    // testFileIO(SPIFFS, "/hello.txt");

    void getSpiffsInfo();
    // getSpiffsInfo();
  private:
};

#endif
