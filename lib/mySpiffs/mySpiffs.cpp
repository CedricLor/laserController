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




void mySpiffs::convertJsonStepsPrettyToUgly(File& prettyFile, const char * _uglyFileName, const char * _cNodeName) {
  Serial.println("mySpiffs::convertJsonStepsPrettyToUgly(): starting.");

  const char _prettyBoxMarkerSize = 14;
  const char _prettyBoxMarker[_prettyBoxMarkerSize] = "\"boxNumber\": ";

  size_t _boxMarkerSize = 15;
  char _boxMarker[_boxMarkerSize];

  bool _boxFound = false;

  const size_t _arrayStarterSize = 11;
  const char _arrayStarter[_arrayStarterSize] = "\"steps\": [";
  bool _arrayStarterMarkerFound = false;
  bool _arrayEndMarkerFound = false;
  byte _startPos;
  uint16_t _endPos;

  char _jObjStartMarker = '{';
  char _jObjEndMarker = '}';

  const size_t _cStepBuffSize = 2000;

  while (prettyFile.available() > 0) {
    // we are still looking for the relevant bloc in the file (the one
    // that follows the line $[BOX: 201).
    // Accordingly, _boxFound is still false.
    if (_boxFound == false) {
      // if we find the box marker: mark _boxFound to true
      if (prettyFile.find(_cNodeName, 3)) {
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): found box %s\n", _cNodeName);
        snprintf(_boxMarker, _boxMarkerSize, "$[BOX: %s]$", _cNodeName);
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): built name %s\n", _boxMarker);
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): about to append it to %s\n", _uglyFileName);
        appendToFile(_uglyFileName, _boxMarker);
        appendToFile(_uglyFileName, "\n");
        _boxFound = true;
        continue;
      }
      // else the box was not found, return
      else {
        // Serial.println("mySpiffs::convertJsonStepsPrettyToUgly(): box not found");
        return;
      }
    }

    // We have now found the relevant box
    // Let's look for the steps array starter marker
    if (_arrayStarterMarkerFound == false) {
      // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): _arrayStarter %s\n", _arrayStarter);
      if (prettyFile.find(_arrayStarter, _arrayStarterSize - 1)) {
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): found the '%s' marker'\n", _arrayStarter);
        _arrayStarterMarkerFound = true;
        _startPos = prettyFile.position();
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());
        continue;
      }
      // else the pretty JSON document is malformed
      else {
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): could not find the '%s' marker'\n", _arrayStarter);
        return;
      }
    }

    // We have now found the relevant box and the steps array starter marker
    // Let's look for the end of the array
    if (_arrayEndMarkerFound == false) {
      // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): _prettyBoxMarker %s\n", _prettyBoxMarker);
      if (prettyFile.find(_prettyBoxMarker, _prettyBoxMarkerSize - 1)) {
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): found the '%s' marker'\n", _prettyBoxMarker);
        _arrayEndMarkerFound = true;
        _endPos = (uint16_t)(prettyFile.position());
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());
        prettyFile.seek(_startPos, SeekSet);
        // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());
        continue;
      }
      // else this is the last box in the file
      prettyFile.seek(0, SeekEnd);
      _endPos = (uint16_t)(prettyFile.position());
      prettyFile.seek(_startPos, SeekSet);
      continue;
    }

    // Now that the steps array starter marker ('"steps": [') has been found,
    // let's read each step, deserialize it, reserialize it ugly and
    // append it to the ugly file.
    if (prettyFile.find(_jObjStartMarker)) {
      char _cStep[_cStepBuffSize];
      // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());
      snprintf(_cStep, _cStepBuffSize, "{%s}", prettyFile.readStringUntil(_jObjEndMarker).c_str());
      // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): current position: %u\n", prettyFile.position());
      if ((uint16_t)(prettyFile.position()) > _endPos) {
        // Serial.println("mySpiffs::convertJsonStepsPrettyToUgly(): about to return");
        return;
      }
      // Serial.printf("mySpiffs::convertJsonStepsPrettyToUgly(): _cStep: %s\n", _cStep);

      const size_t jsonStepCapacity = 905;
      StaticJsonDocument<jsonStepCapacity> _jdStep;
      deserializeJson(_jdStep, _cStep);

      // Serial.print("mySpiffs::convertJsonStepsPrettyToUgly(): _jdStep: ");
      // serializeJson(_jdStep, Serial);
      // Serial.println("\n---");
      char _rStep[_cStepBuffSize];

      serializeJson(_jdStep, _rStep);
      // Serial.print("mySpiffs::convertJsonStepsPrettyToUgly(): _rStep: ");
      // Serial.println(_rStep);

      appendToFile(_uglyFileName, _rStep);
      appendToFile(_uglyFileName, "\n");
    }
  }
}




void mySpiffs::convertJsonFilePrettyToUgly(const char * path, const char * _cNodeName) {
  Serial.printf("mySpiffs::convertJsonFilePrettyToUgly: Reading file: %s\r\n", path);

  // open the pretty file
  File prettyFile = SPIFFS.open(path, FILE_READ);
  if(!prettyFile || prettyFile.isDirectory()){
      Serial.println("mySpiffs::convertJsonFilePrettyToUgly: - failed to open file for reading");
      return;
  }

  // create an ugly file
  char prettyFileName[30];
  strcpy(prettyFileName, prettyFile.name());
  char _uglyFileName[30] = "/";
  strtok(prettyFileName, "-");  // get rid of the beginning of the string
  strcat(_uglyFileName, strtok(NULL, "-")); // add the end "sessions.json"
  // Serial.printf("mySpiffs::convertJsonFilePrettyToUgly: Creating file %s\n", _uglyFileName);

  deleteFile(_uglyFileName);

  File uglyFile;
  uglyFile = SPIFFS.open(_uglyFileName, FILE_WRITE);
  if(!uglyFile){
      Serial.println("mySpiffs::convertJsonFilePrettyToUgly(): - failed to open file for writing");
      prettyFile.close();
      return;
  }

  // Serial.printf("mySpiffs::convertJsonFilePrettyToUgly(): uglyFile.name(): %s\n", uglyFile.name());
  uglyFile.close();

  // // reopen the ugly file for appending
  // uglyFile = SPIFFS.open(_uglyFileName, FILE_APPEND);
  // if(!uglyFile){
  //     Serial.println("mySpiffs::convertJsonFilePrettyToUgly(): - failed to open file for appending");
  //     prettyFile.close();
  //     return;
  // }

  // pass everything to convertJsonStepsPrettyToUgly
  // which will read the pretty file and make it ugly
  convertJsonStepsPrettyToUgly(prettyFile, _uglyFileName, _cNodeName);

  Serial.println("mySpiffs::convertJsonFilePrettyToUgly: closing files");
  // close all the files
  prettyFile.close();
  // uglyFile.close();

  deleteFile(path);

  // readFile(_uglyFileName);
  Serial.println("mySpiffs::convertJsonFilePrettyToUgly: ending");
}





void mySpiffs::readLine(File& file, uint16_t _ui16stepCounter, char* _cStep, const char * _cNodeName) {
  Serial.println("mySpiffs::readLine(): starting");
  char _endLineMarker = '\n';
  uint16_t _ui16InternalStepsCounter = 0;
  char _boxMarker[11] = "$[BOX: ";
  strcat(_boxMarker, _cNodeName);
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




void mySpiffs::readJSONObjLineInFile(const char * path, void (&callBack)(JsonObject&), uint16_t _ui16stepCounter, const char * _cNodeName){
    Serial.printf("mySpiffs::readJSONObjLineInFile: Reading file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readJSONObjLineInFile: - failed to open file for reading");
        return;
    }

    char _cStep[900];
    readLine(file, _ui16stepCounter, _cStep, _cNodeName);

    // reading JSON stuffs
    // capacity = 905 for one step with comments,
    // a little bit larger than what the size of _cStep
    const size_t jsonStepCapacity = 905;
    StaticJsonDocument<jsonStepCapacity> _jdStep;
    DeserializationError err = deserializeJson(_jdStep, _cStep);
    if (err) {
        Serial.print(F("mySpiffs::readJSONObjLineInFile: deserializeJson() failed: "));
        Serial.println(err.c_str());
    }

    // Get a reference to the root object
    JsonObject _joStep = _jdStep.as<JsonObject>();

    callBack(_joStep);

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





// // ma structure config
// struct sConfig {
//   char nom[10];
//   int NumeroPIN;
//   bool defaut;
// };
// sConfig config;
//
// // chemin fichier config dans memoire flash ( SPIFFS )
// const char *fileconfig = "/config/config.json";  // fichier config
//
//
//
//
//
//
//
//
// void sauveFS() {
//   String jsondoc="";
//   DynamicJsonDocument docConfig(capacityConfig);
//   docConfig["nom"]=config.nom;
//   docConfig["pin"]=config.NumeroPIN;
//   docConfig["defaut"]=config.defaut;
//
// File  f = SPIFFS.open(fileconfig, "w");
//   if (!f) {
//     Serial.println("Fichier Config absent - création fichier");
//    }
//    serializeJson(docConfig,jsondoc);
//    f.print(jsondoc);  // sauvegarde de la chaine
//    f.close();
//    Serial.print(jsondoc);
// }
