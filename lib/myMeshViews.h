/*
  myMeshViews.h - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.
*/

#ifndef myMeshViews_h
#define myMeshViews_h

#include "Arduino.h"
#include "ControlerBox.h"

class myMeshViews
{
  public:
    myMeshViews(const char* state);

    static void broadcastStatusOverMesh(const char* state);

    static const short BOXES_I_PREFIX;

  private:
    static char _nodeNameBuf[4];

    static String _createMeshMessage(const char* myStatus);

    static char* _nodeNameBuilder();
};

#endif
