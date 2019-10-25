/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "ControlerBox.h"


// PUBLIC
// Instance Methods

// Constructor
ControlerBox::ControlerBox()
{
  nodeId = 0;
  APIP = {0,0,0,0};
  stationIP = {0,0,0,0};
  ui16NodeName = 254;

  i16BoxActiveState = -1;
  ui32BoxActiveStateStartTime = 0;
  boxActiveStateHasBeenSignaled = true; // to the browser by the interface

  ui32lastRecPirHighTime = 0;

  isNewBoxHasBeenSignaled = true;
  boxDeletionHasBeenSignaled = true;

  // ui16MasterBoxName
  // setters:
  // - here; -> from the globalBaseVariables.gui8DefaultMasterNodeName
  // - in updateMasterBoxName() below. updateMasterBoxName() is called from:
  //    - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  //    - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message)
  // - in mySavedPrefs::loadPreferences()
  // tested or used in:
  // - boxState class (on the laser boxes)
  // - here (printProperties)
  // - in myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) to send various messages
  // - in mySavedPrefs::savePreferences()
  ui16MasterBoxName = globalBaseVariables.gui8DefaultMasterNodeName;

  /** bMasterBoxNameChangeHasBeenSignaled
   *  
   *  Setters:
   *  - here; 
   *  - in updateMasterBoxName() below (called from from stepCollection._applyStep(),
   *    from myMeshController::_updateMyMasterBoxName() and 
   *    myMeshController::_changedBoxConfirmation());
   *  - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) 
   *    (setting it to true, once a message has been sent to the browser).
   *  Testers:
   *  - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) 
   *    (tests whether a change has been made and whether it needs to inform 
   *    the browser). */
  bMasterBoxNameChangeHasBeenSignaled = true;

  sBoxDefaultState = globalBaseVariables.gi16BoxDefaultState;
  sBoxDefaultStateChangeHasBeenSignaled = true;
}






void ControlerBox::updateThisBoxProperties() {
  nodeId = globalBaseVariables.laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = globalBaseVariables.laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = globalBaseVariables.laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  ui16NodeName = globalBaseVariables.gui16NodeName;
  // For this box, i16BoxActiveState, boxActiveStateHasBeenSignaled and ui32BoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (globalBaseVariables.MY_DEBUG == true) {
    Serial.println("ControlerBox::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    this->printProperties(0);
  };
}



void ControlerBox::printProperties(const uint16_t __ui16BoxIndex) {
  const char * _boilerPlate = "ControlerBox::printProperties(): Box n. ";
  Serial.printf("%s[%u] -> nodeId: %u\n", _boilerPlate, __ui16BoxIndex, nodeId);
  Serial.printf("%s[%u] -> APIP:", _boilerPlate, __ui16BoxIndex);Serial.println(APIP.toString());
  Serial.printf("%s[%u] -> stationIP:", _boilerPlate, __ui16BoxIndex);Serial.println(stationIP.toString());
  Serial.printf("%s[%u] -> ui16NodeName: %u\n", _boilerPlate, __ui16BoxIndex, ui16NodeName);

  Serial.printf("%s[%u] -> i16BoxActiveState: %u\n", _boilerPlate, __ui16BoxIndex, i16BoxActiveState);
  Serial.printf("%s[%u] -> ui32BoxActiveStateStartTime: %u\n", _boilerPlate, __ui16BoxIndex, ui32BoxActiveStateStartTime);
  Serial.printf("%s[%u] -> boxActiveStateHasBeenSignaled: %i\n", _boilerPlate, __ui16BoxIndex, boxActiveStateHasBeenSignaled);

  Serial.printf("%s[%u] -> ui32lastRecPirHighTime: %u\n", _boilerPlate, __ui16BoxIndex, ui32lastRecPirHighTime);

  Serial.printf("%s[%u] -> isNewBoxHasBeenSignaled: %i\n", _boilerPlate, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("%s[%u] -> boxDeletionHasBeenSignaled: %i\n", _boilerPlate, __ui16BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("%s[%u] -> sBoxDefaultState: %u\n", _boilerPlate, __ui16BoxIndex, sBoxDefaultState);
  Serial.printf("%s[%u] -> sBoxDefaultStateChangeHasBeenSignaled: %i\n", _boilerPlate, __ui16BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}



void ControlerBox::updateMasterBoxNameFromWeb(const uint16_t _ui16MasterBoxNameFromWeb) {
}



uint16_t ControlerBox::getMasterBoxNameForWeb() {
  return (ui16MasterBoxName - globalBaseVariables.gui16ControllerBoxPrefix);
}



/** const bool ControlerBox::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime)
 *  
 *  Setter for the activeState and associated variables.
 * 
 *  Called from:
 *  - boxStateCollection::_restartTaskPlayBoxState(): for this box, when this box's boxState has changed;
 *  - ControlerBox::updateBoxProperties(): for the other boxes, upon receiving a notification of a boxState
 *    change from the mesh.
*/
const bool ControlerBox::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("ControlerBox::setBoxActiveState(): starting");

  if ( (i16BoxActiveState != _i16boxActiveState) || (ui32BoxActiveStateStartTime != _ui32BoxActiveStateStartTime) ) {
    i16BoxActiveState = _i16boxActiveState;

    boxActiveStateHasBeenSignaled = false;
    /** boxActiveStateHasBeenSignaled setters:
     *  - by default to true upon init (controlerBox constructor);
     *  - to false here;
     *  - to false in myWSReceiver and to true in myWSSender, in the IF: to track change request
     *    coming from the web and whether the states of other boxes have been received;
     *  - to true in myMeshViews (for this box only, upon sending a statusMsg);
     * */

    ui32BoxActiveStateStartTime = _ui32BoxActiveStateStartTime;

    return true;
  }
  // Serial.println("ControlerBox::setBoxActiveState(): over");
  return false;
}






// Setter for the defaultState and associated variables
// Called only from this class (for the other boxes).
void ControlerBox::setBoxDefaultState(const short _sBoxDefaultState) {
  // Serial.println("ControlerBox::setBoxDefaultState(): starting");
  sBoxDefaultState = _sBoxDefaultState;
  sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.println("ControlerBox::setBoxDefaultState(): over");
}




/** Setter for ui32lastRecPirHighTime
 * 
 *  Called from: 
 *  - this class (from the method updateBoxProperties), upon receiving 
 *    an upstream information (limited for the moment to IR High)
 *    message from the other boxes; updateBoxProperties is itself 
 *    called by the _updateOrCreate() method in controllerBoxesCollection, which is 
 *    itself called from the meshController
 *  - the pirController, upon IR high. */
void ControlerBox::setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime) {
  if (_ui32lastRecPirHighTime != ui32lastRecPirHighTime) {
    ui32lastRecPirHighTime = _ui32lastRecPirHighTime;
  }
}







/**void ControlerBox::updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex)
 * 
 *  Updater of the properties of the controller boxes in the mesh
 *  (including this one, at index 0 of the controller boxes array).
 *  
 *  Called from controllerBoxesCollection::_updateOrCreate() exclusively
*/
void ControlerBox::updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex) {
  const char * _subName = "ControlerBox::updateOtherBoxProperties():";
  Serial.printf("%s starting\n", _subName);

  // set the nodeId
  if (nodeId == 0) {
    isNewBoxHasBeenSignaled = false;
    Serial.printf("%s ControlerBoxes[%u].isNewBoxHasBeenSignaled = %i\n", _subName, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  }
  nodeId = _ui32SenderNodeId;
  // Serial.printf("%s ControlerBoxes[%u].nodeId = %u\n", __ui16BoxIndex, _ui32SenderNodeId);

  // set the IPs
  if ( _obj.containsKey("APIP")){ 
    APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);
  }
  if ( _obj.containsKey("StIP")) {
    stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);
  }

  // set the ui16NodeName
  ui16NodeName = _obj["NNa"];

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("boxDefstate")) {
    const short int __senderBoxActiveState = _obj["actSt"];
    const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
    setBoxActiveState(__senderBoxActiveState, __uiSenderBoxActiveStateStartTime);
  }
  
  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("boxDefstate")) {
    const short int __senderBoxDefaultState = _obj["boxDefstate"];
    setBoxDefaultState(__senderBoxDefaultState);
  }

  if (_obj["action"] == "usi" && _obj["key"] == "IR") {
    setBoxIRTimes(_obj["time"].as<uint32_t>());
  }

  // Print out the updated properties
  if (globalBaseVariables.MY_DEBUG == true) {
    Serial.printf("%s Updated box index %u. Calling printProperties().\n", _subName, __ui16BoxIndex);
    printProperties(__ui16BoxIndex);
  }
  Serial.println("%s over");
}






void ControlerBox::_deleteBox() {
  const char * _subName = "ControlerBox::_deleteBox():";
  Serial.printf("%s starting\n", _subName);
  // Serial.printf("%s Received _ui16BoxIndex %u\n", );
  nodeId = 0;
  Serial.printf("%s NodeId reset to 0\n", _subName);
  APIP = {0,0,0,0};
  Serial.printf("%s APIP reset to 0.0.0.0\n", _subName);
  stationIP = {0,0,0,0};
  Serial.printf("%s stationIP reset to 0.0.0.0\n", _subName);
  ui16NodeName = 254;
  Serial.printf("%s ui16NodeName reset to 0\n", _subName);

  i16BoxActiveState = -1;
  Serial.printf("%s i16BoxActiveState reset to -1\n", _subName);
  ui32BoxActiveStateStartTime = 0;
  Serial.printf("%s ui32BoxActiveStateStartTime reset to 0\n", _subName);
  boxActiveStateHasBeenSignaled = true;
  Serial.printf("%s ui32BoxActiveStateStartTime reset to true\n", _subName);

  Serial.printf("%s ui32lastRecPirHighTime reset to 0\n", _subName);
  ui32lastRecPirHighTime = 0;

  isNewBoxHasBeenSignaled = true;
  Serial.printf("%s isNewBoxHasBeenSignaled reset to true\n", _subName);
  boxDeletionHasBeenSignaled = false;
  Serial.printf("%s boxDeletionHasBeenSignaled set to FALSE\n", _subName);

  ui16MasterBoxName = globalBaseVariables.gui8DefaultMasterNodeName;
  Serial.printf("%s ui16MasterBoxName set to %u\n", _subName, ui16MasterBoxName);
  bMasterBoxNameChangeHasBeenSignaled = true;
  Serial.printf("%s bMasterBoxNameChangeHasBeenSignaled set to true\n", _subName);

  sBoxDefaultState = globalBaseVariables.gi16BoxDefaultState;
  Serial.printf("%s sBoxDefaultState set to %i\n", _subName, sBoxDefaultState);
  sBoxDefaultStateChangeHasBeenSignaled = true;
  Serial.printf("%s sBoxDefaultStateChangeHasBeenSignaled set to true\n", _subName);

  Serial.printf("%s over\n", _subName);
}