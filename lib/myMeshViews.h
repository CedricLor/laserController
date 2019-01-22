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

  private:
    static char _nodeNameBuf[4];

    String _createMeshMessage(const char* myStatus);

    char* _nodeNameBuilder();
};

#endif
