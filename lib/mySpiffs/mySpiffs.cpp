/*
  mySpiffs.cpp - access to the spiffs
  Created by Cedric Lor, June 28, 2019.
*/

#include "Arduino.h"
#include "mySpiffs.h"



mySpiffs::mySpiffs() {

}



//////////////////////////////////////////////////////////////////////////////////////////////////
// Directories
//////////////////////////////////////////////////////////////////////////////////////////////////
// List directories
void mySpiffs::listDir(const char * dirname, uint8_t levels){
    Serial.printf("mySpiffs::listDir(): Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
    if(!root){
        Serial.println("mySpiffs::listDir(): - failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("mySpiffs::listDir(): - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}



// create a directory
void mySpiffs::createDir(const char * path){
    Serial.printf("mySpiffs::createDir(): Creating Dir: %s\n", path);
    if(SPIFFS.mkdir(path)){
        Serial.println("mySpiffs::createDir(): Dir created");
    } else {
        Serial.println("mySpiffs::createDir(): mkdir failed");
    }
}




// delete a directory
void mySpiffs::removeDir(const char * path){
    Serial.printf("mySpiffs::removeDir(): Removing Dir: %s\n", path);

    File root = SPIFFS.open(path);
    if(!root){
        Serial.println("mySpiffs::removeDir(): - failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("mySpiffs::removeDir(): - not a directory");
        return;
    }

    // Serial.println("mySpiffs::removeDir(): - opened directory");
    File file = root.openNextFile();
    // Serial.println("mySpiffs::removeDir(): - opened next file");
    // char _extPath[20];
    // strcat(_extPath, path);
    // Serial.println("mySpiffs::removeDir(): - stored path in _exPath");
    // strcat(_extPath, "/");
    // Serial.println("mySpiffs::removeDir(): - added slash to _exPath");

    Serial.println("mySpiffs::removeDir(): - iterating over subfiles");
    while(file){
      // if (strstr(_extPath, file.name()) == (file.name())) {
      deleteFile(file.name());
      Serial.printf("mySpiffs::removeDir(): deleted file %s\n", file.name());
      // }
      file = root.openNextFile();
    }
    Serial.println("mySpiffs::removeDir(): dir is empty");
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// Files
//////////////////////////////////////////////////////////////////////////////////////////////////
// read a file
void mySpiffs::readFile(const char * path){
    Serial.printf("mySpiffs::readFile: Reading file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readFile: - failed to open file for reading");
        return;
    }

    // reading what is in the file and printing it
    Serial.println("mySpiffs::readFile: - read from file:");
    while(file.available()){
        Serial.write(file.read());
    }

    file.close();
}



void mySpiffs::readJSONFile(const char * path){
    Serial.printf("mySpiffs::readJSONFile(): Reading file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readJSONFile(): - failed to open file for reading");
        return;
    }

    // reading JSON stuffs
    // capacity = 900 for one step with comments
    // !! will be too small to read the whole file
    const size_t jsonStepsCapacity = 900;
    StaticJsonDocument<jsonStepsCapacity> jdSteps;
    DeserializationError err = deserializeJson(jdSteps, file);
    if (err) {
        Serial.print(F("mySpiffs::readJSONFile: deserializeJson() failed: "));
        Serial.println(err.c_str());
    }

    // reading what is in the file and printing it
    // Serial.println("mySpiffs::readJSONFile(): - read from file:");
    // while(file.available()){
    //     Serial.write(file.read());
    // }

    file.close();
}





bool mySpiffs::_lookForMyBoxNumberInPrettyFile(File& prettyFile, const char * _uglyFileName, const uint16_t _ui16NodeName, size_t _prettyBoxMarkerSize, char * _prettyBoxMarker) {
  // We are looking for the JSON object in the file that corresponds to this box.
  // This line is marked in the pretty JSON document as follows: ""boxNumber": 201,"
  // It will be rewritten as  $[BOX: 201]$ in the ugly JSON.

  // Building the sought for string  --> "boxNumber": 201,
  snprintf(_prettyBoxMarker, _prettyBoxMarkerSize, "\"boxNumber\": %u,", _ui16NodeName);

  // Looking for the string
  if (prettyFile.find(_prettyBoxMarker, _prettyBoxMarkerSize - 1)) {
    Serial.printf("mySpiffs::_lookForMyBoxNumberInPrettyFile(): found box %s\n", _prettyBoxMarker);

    // Building the box marker to be inserted n the ugly file
    size_t _uglyBoxMarkerSize = 15;
    char _uglyBoxMarker[_uglyBoxMarkerSize];
    snprintf(_uglyBoxMarker, _uglyBoxMarkerSize, "$[BOX: %u]$", _ui16NodeName); // --> $[BOX: 201]$
    // Serial.printf("mySpiffs::_lookForMyBoxNumberInPrettyFile(): built name %s\n", _uglyBoxMarker);
    // Serial.printf("mySpiffs::_lookForMyBoxNumberInPrettyFile(): about to append it to %s\n", _uglyFileName);

    // Appending it to the ugly file
    appendToFile(_uglyFileName, _uglyBoxMarker);
    appendToFile(_uglyFileName, "\n");
    return true;
  }
  // Else the string was not found, return
  // Serial.println("mySpiffs::_lookForMyBoxNumberInPrettyFile(): box number not found");
  return false;
}



uint32_t mySpiffs::_lookForStepsStarterPosInPrettyFile(File& prettyFile, const uint16_t _ui16NodeName) {
    // Let's look for the steps array starter marker and save its position.

    // Building the sought for string  --> "steps": [
    const size_t _arrayStarterSize = 11;
    const char _arrayStarter[_arrayStarterSize] = "\"steps\": [";
    // Creating a variable to store the starting position of the (steps) array for this box
    uint32_t _ui32StartPos;

    // Looking for the string
    // Serial.printf("mySpiffs::_lookForStepsStarterPosInPrettyFile(): _arrayStarter %s\n", _arrayStarter);
    if (prettyFile.find(_arrayStarter, _arrayStarterSize - 1)) {
        // Serial.printf("mySpiffs::_lookForStepsStarterPosInPrettyFile(): found the '%s' marker'\n", _arrayStarter);
        _ui32StartPos = prettyFile.position();
        // Serial.printf("mySpiffs::_lookForStepsStarterPosInPrettyFile(): current position: %u\n", prettyFile.position());
        return _ui32StartPos;
    }
    // else the pretty JSON document is malformed
    // Serial.printf("mySpiffs::_lookForStepsStarterPosInPrettyFile(): could not find the '%s' marker'\n", _arrayStarter);
    return 0;
}



uint32_t mySpiffs::_lookForStepsEndPosInPrettyFile(File& prettyFile, const uint16_t _ui16NodeName, size_t _prettyBoxMarkerSize, char * _prettyBoxMarker) {
    // If there is a box with a name > to this box in the array, there should be a
    // a substring of this form in the file: --> "boxNumber": 202,
    // Let's try to find it, failing which we will just store the end of file position.

    // Creating a variable to store the ending position of the (steps) array for this box
    uint32_t _ui32EndPos;

    // Building the sought for string  --> "boxNumber": 202,
    snprintf(_prettyBoxMarker, _prettyBoxMarkerSize, "\"boxNumber\": %u,", _ui16NodeName + 1);
    // Serial.printf("mySpiffs::_lookForStepsEndPosInPrettyFile(): _prettyBoxMarker %s\n", _prettyBoxMarker);

    // Looking for the string
    // Serial.printf("mySpiffs::_lookForStepsEndPosInPrettyFile(): _prettyBoxMarker %s\n", _prettyBoxMarker);
    if (prettyFile.find(_prettyBoxMarker, _prettyBoxMarkerSize - 1)) {
        // Serial.printf("mySpiffs::_lookForStepsEndPosInPrettyFile(): found the '%s' marker'\n", _prettyBoxMarker);
        // save the current position (with an offset the size of the sought for string)
        _ui32EndPos = prettyFile.position() - _prettyBoxMarkerSize + 1;
        // Serial.printf("mySpiffs::_lookForStepsEndPosInPrettyFile(): current position: %u\n", prettyFile.position());
    } else {
        // else the string was not found => this is the last box
        // go to the end of the file
        prettyFile.seek(0, SeekEnd);
        // save the position at the end of file
        _ui32EndPos = prettyFile.position();
    }
    return _ui32EndPos;
}



void mySpiffs::_copyPrettyStepsToUgly(File& prettyFile, const char * _uglyFileName, uint32_t _ui32EndPos) {
    char _jObjStartMarker = '{';
    char _jObjEndMarker = '}';

    const size_t _cStepBuffSize = 2000;

    while (prettyFile.available() > 0) {
        // Now that the steps array starter marker ('"steps": [') has been found,
        // let's read each step, deserialize it, reserialize it ugly and
        // append it to the ugly file.
        if (prettyFile.find(_jObjStartMarker)) {
        char _cStep[_cStepBuffSize];
        // Serial.printf("mySpiffs::_copyPrettyStepsToUgly(): current position: %u\n", prettyFile.position());
        snprintf(_cStep, _cStepBuffSize, "{%s}", prettyFile.readStringUntil(_jObjEndMarker).c_str());
        // Serial.printf("mySpiffs::_copyPrettyStepsToUgly(): current position: %u\n", prettyFile.position());
        if (prettyFile.position() > _ui32EndPos) {
            // Serial.println("mySpiffs::_copyPrettyStepsToUgly(): about to return");
            return;
        }
        // Serial.printf("mySpiffs::_copyPrettyStepsToUgly(): _cStep: %s\n", _cStep);

        const size_t jsonStepCapacity = 905;
        StaticJsonDocument<jsonStepCapacity> _jdStep;
        deserializeJson(_jdStep, _cStep);

        // Serial.print("mySpiffs::_copyPrettyStepsToUgly(): _jdStep: ");
        // serializeJson(_jdStep, Serial);
        // Serial.println("\n---");
        char _rStep[_cStepBuffSize];

        serializeJson(_jdStep, _rStep);
        // Serial.print("mySpiffs::_copyPrettyStepsToUgly(): _rStep: ");
        // Serial.println(_rStep);

        appendToFile(_uglyFileName, _rStep);
        appendToFile(_uglyFileName, "\n");
        }
    }
}



void mySpiffs::convertJsonStepsPrettyToUgly(File& prettyFile, const char * _uglyFileName, const uint16_t _ui16NodeName) {
    Serial.println("mySpiffs::convertJsonStepsPrettyToUgly(): starting.");


    // variables definition and declaration
    size_t _prettyBoxMarkerSize = 18; // --> "boxNumber": 201,
    char _prettyBoxMarker[_prettyBoxMarkerSize];



    // 1. Look for this file number in the pretty file
    if (!(_lookForMyBoxNumberInPrettyFile(prettyFile, _uglyFileName, _ui16NodeName, _prettyBoxMarkerSize, _prettyBoxMarker))) {
        return;
    }



    // 2. We have now found the relevant box.
    // Let's look for the steps array starter marker and save its position.
    uint32_t _ui32StartPos = _lookForStepsStarterPosInPrettyFile(prettyFile, _ui16NodeName);
    if (!(_ui32StartPos)) {
        return;
    }


    // 3. We have now found the relevant box and the steps array starter marker
    // Let's look for the end of the array
    uint32_t _ui32EndPos = _lookForStepsEndPosInPrettyFile(prettyFile, _ui16NodeName, _prettyBoxMarkerSize, _prettyBoxMarker);
    if (!(_ui32EndPos)) {
        return;
    }



    // 4. go back to the beginning of the array (not the beginning of the file)
    prettyFile.seek(_ui32StartPos, SeekSet);
    // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());


    // 5. Iterate over the JSON objects nested in the array, copy them in memory,
    // clean them up and save them into a new file, specific for this box
    _copyPrettyStepsToUgly(prettyFile, _uglyFileName, _ui32EndPos);
}



void mySpiffs::_createUglyFile(File& prettyFile, char* _uglyFileName) {
    char prettyFileName[30];
    strcpy(prettyFileName, prettyFile.name());
    strtok(prettyFileName, "-");  // get rid of the beginning of the string
    strcat(_uglyFileName, strtok(NULL, "-")); // add the end "sessions.json"
    // Serial.printf("mySpiffs::_createUglyFile: Creating file %s\n", _uglyFileName);

    deleteFile(_uglyFileName); // just in case the ugly file already exists, delete it

    File uglyFile;
    uglyFile = SPIFFS.open(_uglyFileName, FILE_WRITE);
    if(!uglyFile){
        Serial.println("mySpiffs::_createUglyFile(): - failed to open file for writing");
        prettyFile.close();
        return;
    }

    // Serial.printf("mySpiffs::_createUglyFile(): uglyFile.name(): %s\n", uglyFile.name());
    uglyFile.close();
}



void mySpiffs::convertJsonFilePrettyToUgly(const char * path, const uint16_t _ui16NodeName) {
    Serial.printf("mySpiffs::convertJsonFilePrettyToUgly: Reading file: %s\r\n", path);

    // open the pretty file
    File prettyFile = SPIFFS.open(path, FILE_READ);
    if(!prettyFile || prettyFile.isDirectory()){
        Serial.println("mySpiffs::convertJsonFilePrettyToUgly: - failed to open file for reading");
        return;
    }

    // create an ugly file
    char _uglyFileName[30] = "/";
    _createUglyFile(prettyFile, _uglyFileName);

    // pass everything to convertJsonStepsPrettyToUgly
    // which will read the pretty file and make it ugly
    convertJsonStepsPrettyToUgly(prettyFile, _uglyFileName, _ui16NodeName);

    Serial.println("mySpiffs::convertJsonFilePrettyToUgly: closing files");
    // close all the files
    prettyFile.close();
    // uglyFile.close();

    // delete the pretty file
    deleteFile(path);

    // readFile(_uglyFileName);
    Serial.println("mySpiffs::convertJsonFilePrettyToUgly: ending");
}





void mySpiffs::readLine(File& file, uint16_t _ui16stepCounter, char* _cStep, uint16_t _ui16NodeName) {
    Serial.println("mySpiffs::readLine(): starting");
    char _endLineMarker = '\n';
    uint16_t _ui16InternalStepsCounter = 0;
    char _boxMarker[11];
    snprintf(_boxMarker, 11, "%s%u", "$[BOX: ", _ui16NodeName);
    char _cNodeName[4];
    snprintf(_cNodeName, 4, "%u", _ui16NodeName);
    bool _boxFound = false;

    while (file.available() > 0) {
        // we are still looking for the relevant bloc in the file (the one
        // that follows the line $[BOX: 201).
        // Accordingly, _boxFound is still false.
        if (_boxFound == false) {
            // if we find the box marker, mark _boxFound to true and
            // move to the end of the line
            if (file.find(_cNodeName, 3)) {
                Serial.printf("mySpiffs::readLine(): found box %s\n", _cNodeName);
                _boxFound = true;
                file.find(_endLineMarker);
                continue;
            }
            // else the box was not found, return
            else {
                Serial.println("mySpiffs::readLine(): box not found");
                return;
            }
        }
        // This is the case where the relevant NodeName has been found (_boxFound == true).
        // If we are not yet at the line corresponding to the sought for step,
        // - go to the endline marker
        // - increment the step counter
        // - continue in the loop
        if (_ui16InternalStepsCounter < _ui16stepCounter) {
            file.find(_endLineMarker);
            _ui16InternalStepsCounter++;
        }
        // This is the case where the relevant NodeName has been found (_boxFound == true)
        // and where are at the line corresponding to the sought for step.
        // copy the line into the _cStep buffer and return
        if (_ui16InternalStepsCounter == _ui16stepCounter) {
            strcpy(_cStep, file.readStringUntil(_endLineMarker).c_str());
            return;
        }
    }
}




void mySpiffs::readStepInFile(const char * path, uint16_t _ui16stepCounter, char * _cStep, uint16_t _ui16NodeName){
// void mySpiffs::readJSONObjLineInFile(const char * path, void (&callBack)(JsonObject&), uint16_t _ui16stepCounter, const char * _cNodeName){
    Serial.printf("mySpiffs::readJSONObjLineInFile: Reading file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readJSONObjLineInFile: - failed to open file for reading");
        return;
    }

    readLine(file, _ui16stepCounter, _cStep, _ui16NodeName);

    file.close();
}



// write a new file // overwrites an existing file
void mySpiffs::writeFile(const char * path, const char * message){
    Serial.printf("mySpiffs::writeFile(): Writing file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if(!file){
        Serial.println("mySpiffs::writeFile(): - failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("mySpiffs::writeFile(): - file written");
    } else {
        Serial.println("mySpiffs::writeFile(): - write failed");
    }
    file.close();
}



// append to an existing file
void mySpiffs::appendToFile(const char * path, const char * message){
    Serial.printf("mySpiffs::appendToFile(): Appending to file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if(!file){
        Serial.println("mySpiffs::appendToFile(): - failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("mySpiffs::appendToFile(): - message appended");
    } else {
        Serial.println("mySpiffs::appendToFile(): - append failed");
    }
    file.close();
}



// rename an existing file
void mySpiffs::renameFile(const char * path1, const char * path2){
    Serial.printf("mySpiffs::renameFile(): Renaming file %s to %s\r\n", path1, path2);
    if (SPIFFS.rename(path1, path2)) {
        Serial.println("mySpiffs::renameFile(): - file renamed");
    } else {
        Serial.println("mySpiffs::renameFile(): - rename failed");
    }
}



// delete a file
void mySpiffs::deleteFile(const char * path){
    Serial.printf("mySpiffs::deleteFile(): Deleting file: %s\r\n", path);
    if(SPIFFS.remove(path)){
        Serial.println("mySpiffs::deleteFile(): - file deleted");
    } else {
        Serial.println("mySpiffs::deleteFile(): - delete failed");
    }
}


// // test file IO
void mySpiffs::testFileIO(const char * path){
    Serial.printf("mySpiffs::testFileIO(): Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = SPIFFS.open(path, FILE_WRITE);
    if(!file){
        Serial.println("mySpiffs::testFileIO(): - failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("mySpiffs::testFileIO(): - writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    uint32_t end = millis() - start;
    Serial.printf("\nmySpiffs::testFileIO():  - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = SPIFFS.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("mySpiffs::testFileIO():  - reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("\nmySpiffs::testFileIO(): - %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("mySpiffs::testFileIO(): - failed to open file for reading");
    }
}
