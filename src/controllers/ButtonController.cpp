#include "Arduino.h"
#include "ButtonController.h"

//constructor
ButtonController::ButtonController(byte _Pin)
{
  devicePin = _Pin;       // set the gpio pin of the device
  pinMode(devicePin, INPUT_PULLUP);   // set the mode of the pin to INPUT
  digitalWrite(devicePin, LOW); // send LOW signal to the device in rder to turn it OFF
  currentState = LOW;     // set device state to LOW
}

//get the current state of the input device
byte ButtonController::getCurrentState()
{
  byte c = digitalRead(devicePin); // send HIGH signal to the device in order to trun it ON
  if (c == HIGH)
  {
    currentState = HIGH;
  }
  else
  {
    currentState = LOW;
  }
  
  return c;
}