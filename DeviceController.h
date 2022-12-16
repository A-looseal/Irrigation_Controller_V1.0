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
