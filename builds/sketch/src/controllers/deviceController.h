#line 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\src\\controllers\\DeviceController.h"
// DeviceController.h


#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <Arduino.h>

class DeviceController {
  private:
    byte devicePin;

  public:
    bool deviceCurrentState;
    int deviceCurrentCycleCount;

    DeviceController(byte devicePin);
    void turnDeviceOn();
    void turnDeviceOff();
    bool getDeviceState();
};

#endif
