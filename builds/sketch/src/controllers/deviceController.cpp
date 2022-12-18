#line 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\src\\controllers\\DeviceController.cpp"
#include "Arduino.h"
#include "DeviceController.h"

DeviceController::DeviceController(byte _devicePin)
{
  devicePin = _devicePin;       // set the gpio pin of the device
  pinMode(devicePin, OUTPUT);   // set the mode of the pin to OUTPUT
  digitalWrite(devicePin, LOW); // send LOW signal to the device in rder to turn it OFF
  deviceState = LOW;            // set device state to LOW
  enableAlarm_A1 = true;
  enableAlarm_A2 = false;
  enableAlarm_A3 = false;
  enableAlarm_A4 = false;
  enableAlarm_A5 = false;
}

void DeviceController::turnDeviceOn()
{
  digitalWrite(devicePin, HIGH); // send HIGH signal to the device in order to trun it ON
  deviceState = HIGH;            // set device state to LOW
}

void DeviceController::turnDeviceOff()
{
  digitalWrite(devicePin, LOW); // send LOW signal to the device in order to trun it OFF
  deviceState = LOW;            // set device state to LOW
}

bool DeviceController::getDeviceState()
{
  return deviceState;
}