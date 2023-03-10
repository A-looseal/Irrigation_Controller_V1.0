// DeviceController.h


#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <Arduino.h>

class DeviceController {
  private:
    byte devicePin;

  public:
  bool alarmState;
  byte lastAlarmTriggered;
  bool enableAlarm_A1;
  bool enableAlarm_A2;
  bool enableAlarm_A3;
  bool enableAlarm_A4;
  bool enableAlarm_A5;
  bool enableAlarm_A6;

    bool deviceState;
    byte currentCycleCount;
    int totalCycleCount;


    DeviceController(byte devicePin);
    void turnDeviceOn();
    void turnDeviceOff();
    bool getDeviceState();
};

#endif
