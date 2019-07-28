/*
  mySpiffs.cpp - access to the spiffs
  Created by Cedric Lor, June 28, 2019.
*/

#include "Arduino.h"
#include "mySpiffs.h"



mySpiffs::mySpiffs() {

}

// List directories
void mySpiffs::listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
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
void mySpiffs::createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}


// delete a directory
void mySpiffs::removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}


// read a file
void mySpiffs::readFile(fs::FS &fs, const char * path){
    Serial.printf("mySpiffs::readFile: Reading file: %s\r\n", path);

    // File file = fs.open(path);
    File file = SPIFFS.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println("mySpiffs::readFile: - failed to open file for reading");
        return;
    }

    // reading JSON stuffs (from https://byfeel.info/eeprom-ou-spiffs/)
    const size_t jsonStepsCapacity = JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(8) + 2*JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(15) + 3280;
    StaticJsonDocument<jsonStepsCapacity> jdSteps;
    DeserializationError err = deserializeJson(jdSteps, file);
    if (err) {
        Serial.print(F("mySpiffs::readFile: deserializeJson() failed: "));
        Serial.println(err.c_str());
    }

    // // system exemple from SPIFFS esp32-arduino exemple
    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }

    // alloting values (from https://byfeel.info/eeprom-ou-spiffs/)
    // affectation des valeurs , si existe pas on place une valeur par defaut
    // strlcpy(config.nom,docConfig["nom"] | "byfeel",sizeof(config.nom));
    // config.NumeroPIN = jdSteps["pin"] | 10;
    // config.defaut = jdSteps["defaut"] | true;
    // fermeture fichier

    file.close();
}


// write a new file
void mySpiffs::writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
    file.close();
}



// append to an existing file
void mySpiffs::appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}



// rename an existing file
void mySpiffs::renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}



// delete a file
void mySpiffs::deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}


// // test file IO
void mySpiffs::testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
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
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
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
