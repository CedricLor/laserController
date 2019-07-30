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


void mySpiffs::readLine(File& file, uint16_t _ui16stepCounter, char* _cStep) {
  int16_t _i16Index = 0;
  char _endLineMarker = '\n';
  char _cReadChar;
  boolean _bNewData = false;
  uint16_t _ui16InternalStepsCounter = 0;
  // const byte numChars = 32;

  while (file.available() > 0 && _bNewData == false) {
    if ((_ui16InternalStepsCounter < _ui16stepCounter) && (file.read() != _endLineMarker)) {
      _ui16InternalStepsCounter++;
    }
    if (_ui16InternalStepsCounter == _ui16stepCounter) {
      strcpy(_cStep, file.readStringUntil('\n').c_str());

      // _cReadChar = file.read();
      // if (_cReadChar != _endLineMarker) {
      //     _cStep[_i16Index] = _cReadChar;
      //     _i16Index++;
      //     // if (ndx >= numChars) {
      //     //     ndx = numChars - 1;
      //     // }
      //   }
        // else {
        //     _cStep[_i16Index] = '\0'; // terminate the string
        //     _i16Index = 0;
        //     _bNewData = true;
        //     _ui16InternalStepsCounter++;
        //     break;
        // }
    }
    if (_ui16InternalStepsCounter > _ui16stepCounter){
      break;
    }
  }
}

void mySpiffs::readJSONObjLineInFile(const char * path, void (&callBack)(JsonObject&), uint16_t _ui16stepCounter){
    Serial.printf("mySpiffs::readJSONObjLineInFile: Reading file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readJSONObjLineInFile: - failed to open file for reading");
        return;
    }

    char _cStep[900];
    mySpiffs::readLine(file, _ui16stepCounter, _cStep);

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
