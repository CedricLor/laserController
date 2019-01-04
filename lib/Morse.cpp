/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2007.
*/

#include "Arduino.h"
#include "myWebServerControler.h"

myWebServerControler::myWebServerControler(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void myWebServerControler::dot()
{
  digitalWrite(_pin, HIGH);
  delay(250);
  digitalWrite(_pin, LOW);
  delay(250);
}

void myWebServerControler::dash()
{
  digitalWrite(_pin, HIGH);
  delay(1000);
  digitalWrite(_pin, LOW);
  delay(250);
}
