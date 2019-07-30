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

    void listDir(const char * dirname, uint8_t levels);
    // listDir("/", 0);
    void createDir(const char * path);
    // void createDir("/mydir");
    void removeDir(const char * path);
    // void removeDir("/mydir");

    void readFile(const char * path);
    // readFile("/hello.txt")
    void readJSONFile(const char * path);

    void readLine(File& file, uint16_t _ui16stepCounter, char* _cStep, const char * _cNodeName);

    void readJSONObjLineInFile(const char * path/*, void (&callBack)(JsonObject&)*/, uint16_t _ui16stepCounter);

    void writeFile(const char * path, const char * message);
    // writeFile("/hello.txt", "Hello ");
    void appendToFile(const char * path, const char * message);
    // appendFile("/hello.txt", "World!\n");
    void renameFile(const char * path1, const char * path2);
    // renameFile("/hello.txt", "/world.txt");
    void deleteFile(const char * path);
    // deleteFile("/hello.txt");
    void testFileIO(const char * path);
    // testFileIO("/hello.txt");

    void getSpiffsInfo();
    // getSpiffsInfo();
  private:
};

#endif
