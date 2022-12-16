#include "src/controllers/DeviceController.h"
#include "src/controllers/ButtonController.h"

#include <millisDelay.h> //interval timer library

#define ENABLE_SERIAL_OUTPUT

const unsigned long TEST_DEVICE_INTERVAL = 10000; // in ms

#pragma region DEVICE CONSTRUCTORS
// WATER PUMP CONSTRUCTORS
bool waterPumpAlarmState = LOW;
millisDelay waterPumpTimer; // create a new interval timer object to track the on interval
DeviceController waterPump(2);

// OXYGEN PUMP CONSTRUCTORS
bool oxygenPumpState = LOW;
millisDelay oxygenPumpTimer; // create a new interval timer object to track the on interval
DeviceController oxygenPump(3);

// WHITE LIGHT CONSTRUCTORS
bool whiteLightAlarmState = LOW;
millisDelay whiteLightTimer; // create a new interval timer object to track the on interval
DeviceController whiteLight(4);

// RED LIGHT CONSTRUCTORS
bool redLightAlarmState = LOW;
millisDelay redLightTimer; // create a new interval timer object to track the on interval
DeviceController redLight(5);

// TEST DEVICE CONSTRUCTORS
bool testDeviceAlarmState = LOW;
millisDelay testDeviceTimer;     // create a new interval timer object to track the on interval
DeviceController testDevice(13); // create a new device object
#pragma endregion

#pragma region HID CONSTRUCTORS
ButtonController safetySwitch(12);
ButtonController pumpOverride(11);
#pragma endregion

void setup()
{
    Serial.begin(9600);

    Serial.println("Enter R to start or S to stop the timer.");
}

void loop()
{
    checkSafetyButton();

    checkTestDeviceAlarms();

    toggleTestDevice();

    printTestDeviceState();
    Serial.print("test device alarm state: ");
    Serial.println(testDeviceAlarmState);
    Serial.println("----------------");
    Serial.println("----------------");
    delay(1000);
}

// Check the set alarms and see if any of them are triggered
void checkTestDeviceAlarms()
{
    char c = getInput();

    // TURN DEVICE ON IF IT IS ALARM TIME
    if ((c == 'R') || (c == 'r') && (testDeviceAlarmState == false))
    {
#ifdef ENABLE_SERIAL_OUTPUT
        Serial.println("Test Device Alarm Triggered!");
        Serial.println("Turning 'Test Device' ON.");
#endif
        testDeviceAlarmState = true;                 // set the alarm state to ON so we dont repeatidly run this code
        testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
        testDevice.turnDeviceOn();                   // turn ON the device for the first time
    }

    // TURN DEVICE OFF IF IT IS NOT ALARM TIME
    if ((c == 'S') || (c == 's') && (testDeviceAlarmState == true))
    {

#ifdef ENABLE_SERIAL_OUTPUT
        Serial.println("Test Device Alarm reset!");
        Serial.println("Turning 'Test Device' OFF.");
#endif
        testDeviceAlarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
        testDeviceTimer.stop();       // stop the ON interval timer
        testDevice.turnDeviceOff();   // turn OFF the device indefenitley
        testDevice.deviceCurrentCycleCount = 0;
    }
}

// Toggle the device state on/off forever
void toggleTestDevice()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (testDeviceTimer.justFinished())
    {                             // if the interval timer has finished
        testDeviceTimer.repeat(); // repeat the interval timer

        testDevice.deviceCurrentState = !testDevice.deviceCurrentState; // toggle device current state

        // toggle device
        if (testDevice.deviceCurrentState)
        {                              // if the device state is ON
            testDevice.turnDeviceOn(); // toggle the device ON
        }
        else
        {                                         // else if the state is OFF
            testDevice.turnDeviceOff();           // toggle the device OFF
            testDevice.deviceCurrentCycleCount++; // increase current cycle count
        }
    }
}

// print test device status to the serial console
void printTestDeviceState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((testDeviceAlarmState == HIGH)) // only print this if the alarm is triggered
    {
        if ((testDevice.deviceCurrentState == LOW)) // if the device state is OFF
        {
            Serial.println("Test Device currently OFF.");
        }
        if ((testDevice.deviceCurrentState == HIGH)) // if the device state is ON
        {
            Serial.println("Test Device currently ON.");
        }

        Serial.print("Test Device current cyle count: ");
        Serial.println(testDevice.deviceCurrentCycleCount);

        Serial.print("Test Device interval remaining: ");
        Serial.println(testDeviceTimer.remaining());
    }
#endif
}

//
void checkSafetyButton()
{
    if (safetySwitch.getCurrentState() == LOW) // backwards logic
    {
        Serial.println("Safety Switch has been ACTIVATED!");
        testDevice.turnDeviceOff();
    }
}

// get input from the serial console
char getInput()
{
    char c = 0;
    if (Serial.available())
    {
        c = Serial.read();
        while (Serial.available())
        {
            Serial.read(); // clear rest on input
        }
    }
    return c;
}