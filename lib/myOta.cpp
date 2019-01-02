/*
  myOta.h - Library for storing OTA related code in laserController.
  Created by Cedric Lor, January 1, 2019.
  Released into the public domain.
*/

#include "Arduino.h"
#include "myOta.h"

myOta::myOta(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void myOta::dot()
{
  digitalWrite(_pin, HIGH);
  delay(250);
  digitalWrite(_pin, LOW);
  delay(250);
}

void myOta::dash()
{
  digitalWrite(_pin, HIGH);
  delay(1000);
  digitalWrite(_pin, LOW);
  delay(250);
}
