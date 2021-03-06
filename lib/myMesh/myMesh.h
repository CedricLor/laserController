/*
  myMesh.h - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.
*/

#ifndef myMesh_h
#define myMesh_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <myMeshController.h> // => includes myMeshStarter and myMeshViews

class myMesh
{
  public:
    myMesh();

    static void init();
    static void start();

    static Task tChangedConnection;
    static Task tIamAloneTimeOut;
    static Task tPrintMeshTopo;
    static Task tUpdateCBArrayOnChangedConnections;
    static Task tSaveNodeMap;

  private:

    /** ----- receivedCallback stack -----
     **********************************************/

    /** --> painlessMesh receivedCallback */
    static void receivedCallback( uint32_t from, String &msg);



    /** ----- newConnectionCallback stack -----
     **********************************************/

    /** _tcbSendStatusOnNewConnection() is the default callback
     *  of tChangedConnection().
     * 
     *  It sends a statusMsg upon detecting a new connection. */
    static void _tcbSendStatusOnNewConnection();
    /** --> painlessMesh newConnectionCallback */
    static void newConnectionCallback(uint32_t nodeId);



    /** ----- droppedConnectionCallback stack ----- 
     **********************************************/

    /** _tcbSendNotifOnDroppedConnection() is the alternative callback
     *  of tChangedConnection.
     * 
     *  It is attached to tChangedConnection, as a replacement of 
     *  _tcbSendStatusOnNewConnection() when the droppedConnectionCallback
     *  is called a few seconds after the changedConnectionCallback().
     * 
     *  It sends a notification to the mesh, informing the other boxes
     *  of the disconnection of a direct neighbour. */
    static void _tcbSendNotifOnDroppedConnection(uint32_t nodeId);  
    /** --> painlessMesh changedConnectionCallback */
    static void droppedConnectionCallback(uint32_t nodeId);


    /** ----- changedConnectionCallback stack -----
     **********************************************/
    static void _odtcbChangedConnection();
    /** --> painlessMesh changedConnectionCallback */
    static void changedConnectionCallback();

    // static void nodeTimeAdjustedCallback(int32_t offset);
    // static void delayReceivedCallback(uint32_t from, int32_t delay);


    // Helpers
    
    /** tIamAloneTimeOut() was implemented as a way to solve the weird
     * behavior of painlessMesh where subnodes stopped scanning after being 
     * disconnected and left alone. 
     * 
     * It was simply restarting the mesh. */
    static void _tcbIamAloneTimeOut();
    /** static bool _IamAlone stores the state returned by IamAlone() */
    static bool IamAlone();

    // For debug purposes
    static void _printNodeListAndTopology();

    // To update the the controller boxes' array
    static bool _oetcbUpdateCBOnChangedConnections();
    static void _tcbUpdateCBOnChangedConnections();
    static void _odtcbUpdateCBOnChangedConnections();
    static void _deleteBoxesFromCBArray(std::map<uint32_t, uint16_t> &_nodeMap);
    static uint16_t _deleteBoxFromCBArray(uint32_t nodeId);

    // To keep track of the mesh layout
    static void _tcbSaveNodeMap();
    static std::map<uint32_t, uint16_t> _nodeMap;
    static void _saveNodeMap();
};

#endif


