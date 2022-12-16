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
