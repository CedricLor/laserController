/*
  controllerBoxThis.h - controllerBoxThis integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "controllerBoxThis.h"

controllerBoxThis::controllerBoxThis():
  globBaseVars(globalBaseVariables),
  thisSignalHandler(),
  thisCtrlerBox(thisSignalHandler.ctlBxColl.controllerBoxesArray.at(0)),
  thisMeshViews(myMeshViews(thisSignalHandler.ctlBxColl.controllerBoxesArray.at(0))),
  thisPirController(pirController(thisSignalHandler, tSetPirTimeStampAndBrdcstMsg, tPirSpeedBumper, thisMeshViews, 12))
{
  tReboot.set(3000, TASK_ONCE, NULL, NULL, [&](){_reboot();});
}





void controllerBoxThis::updateThisBoxProperties() {
  thisCtrlerBox.networkData.nodeId = globalBaseVariables.laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  thisCtrlerBox.APIP = globalBaseVariables.laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  thisCtrlerBox.stationIP = globalBaseVariables.laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  thisCtrlerBox.ui16NodeName = globalBaseVariables.gui16NodeName;
  // For this box, i16BoxActiveState, boxActiveStateHasBeenSignaled and ui32BoxActiveStateStartTime are updated
  // by calls to setBoxActiveState from boxState
  if (globalBaseVariables.MY_DEBUG == true) {
    Serial.println("controllerBoxThis::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    thisCtrlerBox.printProperties(0);
  };
}





void controllerBoxThis::addLaserTasks() {

  // conditional?
  globalBaseVariables.scheduler.addTask(thisSignalHandler.tSetBoxState);
  if ((globalBaseVariables.isInterface == false) || (globalBaseVariables.isRoot == false)) {
    globalBaseVariables.scheduler.addTask(tSetPirTimeStampAndBrdcstMsg);
    globalBaseVariables.scheduler.addTask(tPirSpeedBumper);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._stepColl.tPreloadNextStep);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl.tPlayBoxState);

    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.tPreloadNextLaserSequence);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences._bars.tPreloadNextBar);

    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences.tPlayLaserSequence);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences._bars.tPlayBar);
    globalBaseVariables.scheduler.addTask(thisSignalHandler.thisBxStateColl._laserSequences._bars.getNotes().tPlayNote);
  }

}





void controllerBoxThis::_reboot() {
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}