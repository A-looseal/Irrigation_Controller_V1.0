#line 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\src\\controllers\\ButtonController.h"
// ButtonController.h


#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H

#include <Arduino.h>

class ButtonController {
  private:
    byte devicePin;

  public:
    bool currentState;

    ButtonController(byte Pin);
    byte getCurrentState();
};

#endif
