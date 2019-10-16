/*
  mySpiffs.h - access to the spiffs
  Created by Cedric Lor, June 28, 2019.
*/

#ifndef mySpiffs_h
#define mySpiffs_h

#include "Arduino.h"
#include <FS.h>
#include <SPIFFS.h>
#include <global.h>
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

    void convertJsonStepsPrettyToUgly(File& prettyFile, const char * _uglyFileName, const uint16_t _ui16NodeName);
    void convertJsonFilePrettyToUgly(const char * path, const uint16_t _ui16NodeName);

    void readLine(File& file, uint16_t _ui16stepCounter, char* _buffer, size_t _buff_size);
    bool readItemInFile(const char * path, uint16_t _ui16stepCounter, StaticJsonDocument<905> & _jd);
    uint16_t numberOfLinesInFile(const char * path);

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
    bool _lookForMyBoxNumberInPrettyFile(File& prettyFile, const uint16_t _ui16NodeName, size_t _prettyBoxMarkerSize, char * _prettyBoxMarker);
    bool _insertBoxNumberFromPrettyFileToUglyFile(File& prettyFile, const char * _uglyFileName, const uint16_t _ui16NodeName, size_t _prettyBoxMarkerSize, char * _prettyBoxMarker);
    uint32_t _lookForStepsStarterPosInPrettyFile(File& prettyFile, const uint16_t _ui16NodeName);
    uint32_t _lookForStepsEndPosInPrettyFile(File& prettyFile, const uint16_t _ui16NodeName, size_t _prettyBoxMarkerSize, char * _prettyBoxMarker);
    void _copyPrettyStepsToUgly(File& prettyFile, const char * _uglyFileName, uint32_t _ui32EndPos);
    void _createUglyFile(File& prettyFile, char* _uglyFileName);
};

#endif
