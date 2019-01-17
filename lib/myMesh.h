/*
  myMesh.h - Library web server controller related functions.
  Created by Cedric Lor, January 17, 2019.
*/

#ifndef myMesh_h
#define myMesh_h

#include "Arduino.h"

class myMesh
{
  public:
    myMesh(/*int pin*/);
    void dot();
    void dash();
  private:
    // int _pin;
};

#endif
