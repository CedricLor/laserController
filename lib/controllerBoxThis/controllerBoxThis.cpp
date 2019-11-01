/*
  controllerBoxThis.h - controllerBoxThis integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "controllerBoxThis.h"

controllerBoxThis::controllerBoxThis():
  globBaseVars(globalBaseVariables),
  thisCtrlerBox(globBaseVars.gui16NodeName),
  thisMeshViews(thisCtrlerBox),
  thisSignalHandler(thisCtrlerBox, thisMeshViews),
  thisPirController(thisCtrlerBox, thisSignalHandler, tSetPirTimeStampAndBrdcstMsg, tPirSpeedBumper, thisMeshViews, 12)
{
  tReboot.set(3000, TASK_ONCE, NULL, NULL, [&](){_reboot();});
}




/** void controllerBoxThis::updateThisBoxNetWorkProperties()
 * 
 *  actualizes the box network properties (i.e. its nodeId, its station IP and 
 *  its APIP).
 * 
 *  thisCtrlerBox is the ControlerBox instance representing this box. */
void controllerBoxThis::_updateThisBoxNetWorkProperties() {
  thisCtrlerBox.networkData.nodeId = globalBaseVariables.laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  thisCtrlerBox.networkData.stationIP = globalBaseVariables.laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  thisCtrlerBox.networkData.APIP = globalBaseVariables.laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
}





/** void controllerBoxThis::updateThisBoxNodeName()
 * 
 *  actualizes this box nodeName (i.e. the nodeName is set box by box in globalBaseVariables).
 * 
 *  thisCtrlerBox is the ControlerBox instance representing this box. */
void controllerBoxThis::_updateThisBoxNodeName() {
  thisCtrlerBox.ui16NodeName = globalBaseVariables.gui16NodeName;
}





void controllerBoxThis::updateThisBoxProperties() {
  _updateThisBoxNetWorkProperties();
  _updateThisBoxNodeName();
  // For this box, i16BoxActiveState, boxActiveStateHasBeenSignaled and ui32BoxActiveStateStartTime are updated
  // by calls to setBoxActiveState from boxState
  if (globalBaseVariables.MY_DEBUG == true) {
    Serial.println("controllerBoxThis::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    thisCtrlerBox._printProperties(0);
  };
}





void controllerBoxThis::addLaserTasks() {

  // conditional?
  globalBaseVariables.scheduler.addTask(thisSignalHandler.tSetBoxState);
  if (globalBaseVariables.hasLasers == true) {
    globalBaseVariables.scheduler.addTask(tSetPirTimeStampAndBrdcstMsg);
    globalBaseVariables.scheduler.addTask(tPirSpeedBumper);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._stepColl.tPreloadNextStep);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl.tPlayBoxState);

    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.tPreloadNextLaserSequence);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.getBars().tPreloadNextBar);

    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.tPlayLaserSequence);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.getBars().tPlayBar);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.getBars().getNotes().tPlayNote);
  }

}





void controllerBoxThis::_reboot() {
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}