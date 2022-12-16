#include "Arduino.h"
#include "DeviceController.h"

DeviceController::DeviceController(byte _devicePin)
{
  devicePin = _devicePin;       // set the gpio pin of the device
  pinMode(devicePin, OUTPUT);   // set the mode of the pin to OUTPUT
  digitalWrite(devicePin, LOW); // send LOW signal to the device in rder to turn it OFF
  deviceCurrentState = LOW;     // set device state to LOW
}

void DeviceController::turnDeviceOn()
{
  digitalWrite(devicePin, HIGH); // send HIGH signal to the device in order to trun it ON
  deviceCurrentState = HIGH;     // set device state to LOW
}

void DeviceController::turnDeviceOff()
{
  digitalWrite(devicePin, LOW); // send LOW signal to the device in order to trun it OFF
  deviceCurrentState = LOW;     // set device state to LOW
}

bool DeviceController::getDeviceState()
{
  return deviceCurrentState;
}