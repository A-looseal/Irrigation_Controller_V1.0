#pragma region IRRIGATION CONTROLLER V1 CODE BASE
#pragma region INCLUDES
#include "src/settings/TestDeviceSettings.h"
#include "src/settings/WaterPumpSettings.h"
#include "src/settings/OxygenPumpSettings.h"
#include "src/settings/WhiteLightSettings.h"
#include "src/settings/RedLightSettings.h"
//
#include "src/controllers/DeviceController.h"
#include "src/controllers/ButtonController.h"
#include "src/tools/timeTools.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>      // Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <millisDelay.h> //interval timer library
#pragma endregion

#pragma region ENABLE /DISABLE_DEVICES
// #define ENABLE_TEST_DEVICE // use this line to enable/dissable use of the TEST DEVICE
#define ENABLE_WATER_PUMP  // use this line to enable/dissable use of the WATER PUMP
#define ENABLE_OXYGEN_PUMP // use this line to enable/dissable use of the OXYGEN PUMP
#define ENABLE_WHITE_LIGHT // use this line to enable/dissable use of the WHITE LIGHT
#define ENABLE_RED_LIGHT   // use this line to enable/dissable use of the RED_LIGHT
#pragma endregion
#pragma region DEVICE_CONSTRUCTORS
// WATER PUMP CONSTRUCTORS
millisDelay waterPumpTimer; // create a new interval timer object to track the on interval
DeviceController waterPump(2);

// OXYGEN PUMP CONSTRUCTORS
millisDelay oxygenPumpTimer; // create a new interval timer object to track the on interval
DeviceController oxygenPump(3);

// WHITE LIGHT CONSTRUCTORS
millisDelay whiteLightTimer; // create a new interval timer object to track the on interval
DeviceController whiteLight(4);

// RED LIGHT CONSTRUCTORS
millisDelay redLightTimer; // create a new interval timer object to track the on interval
DeviceController redLight(5);

// TEST DEVICE CONSTRUCTORS
millisDelay testDeviceTimer;     // create a new interval timer object to track the on interval
DeviceController testDevice(13); // create a new device object
#pragma endregion

#pragma region SYSTEM_DEFENITIONS
#define AMOUNT_OF_DEVICES 4
// #define RESET_RTC_TIME
// #define ENABLE_SERIAL_OUTPUT // use this line to enable/dissable usage of the SERIAL CONSOLE
#define ENABLE_OLED_OUTPUT // use this line to enable/disable usage of the OLED device
#define BAUD_RATE 9600
#define STARTUP_DELAY 1000
#pragma endregion
#pragma region SYSTEM_INTERVAL TIMERS
// millisDelay timer_oled_displayGeneralInfo;
millisDelay timer_oled_CycleAllDeviceInfo;
byte timer_oledCurrentState = 0;
#define OLED_CYCLE_INTERVAL 5000
#define SYSTEM_UPDATE_INTERVAL 1000
#pragma endregion

#pragma region HID_CONSTRUCTORS
ButtonController safetySwitch(11);
ButtonController pumpOverride(10);
#pragma endregion

#pragma region I2C_DEVICES
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS3231 rtc;
DateTime now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#pragma endregion

#pragma region MAIN
void setup()
{
    setupSerialConnection(BAUD_RATE, STARTUP_DELAY);
    // setup and establish a connection to the OLED DISPLAY
    setupDisplay(STARTUP_DELAY);
    setupRTC(STARTUP_DELAY);

    testDevice.turnDeviceOff();
    waterPump.turnDeviceOff();
    oxygenPump.turnDeviceOff();
    whiteLight.turnDeviceOff();
    redLight.turnDeviceOff();

    // turn on test device alarms
    testDevice.enableAlarm_A1 = true;
    testDevice.enableAlarm_A2 = true;
    testDevice.enableAlarm_A3 = true;
    testDevice.alarmState = HIGH; // make sure the alarms are not triggered on startup

    // turn on Water Pump alarms
    waterPump.enableAlarm_A1 = true;
    waterPump.enableAlarm_A2 = false;
    waterPump.enableAlarm_A2 = false;
    waterPump.alarmState = HIGH; // make sure the alarms are not triggered on startup

    // turn on Oxygen Pump alarms
    oxygenPump.enableAlarm_A1 = true;
    oxygenPump.enableAlarm_A2 = false;
    oxygenPump.enableAlarm_A3 = false;
    oxygenPump.alarmState = HIGH; // make sure the alarms are not triggered on startup

    // turn on White Light alarms
    whiteLight.enableAlarm_A1 = true;
    whiteLight.enableAlarm_A2 = true;
    whiteLight.enableAlarm_A3 = true;
    whiteLight.enableAlarm_A4 = true;
    whiteLight.alarmState = HIGH; // make sure the alarms are not triggered on startup

    // turn on Red Light alarms
    redLight.enableAlarm_A1 = true;
    redLight.enableAlarm_A2 = true;
    redLight.enableAlarm_A3 = true;
    redLight.enableAlarm_A4 = true;
    redLight.alarmState = HIGH; // make sure the alarms are not triggered on startup

    delay(5000);
}

void loop()
{
    now = rtc.now(); // get the time from the RTC module

    // checkSafetySwitch();         //check the current state of the safety switch

#ifdef ENABLE_SERIAL_OUTPUT // serial output header
    Serial.println("----------------");
    serial_dispalyCurrentTime();
#endif

#pragma region EXECUTE DEVICE LOGIC
#ifdef ENABLE_TEST_DEVICE
    checkTestDeviceAlarms();
    // TODO: check if device off timer is enabled.
    // TODO: check if device repeat toggle is enabled.
    toggleTestDevice();

#ifdef ENABLE_SERIAL_OUTPUT
    serial_displayTestDeviceState();

    if (testDevice.alarmState == HIGH)
    {
        Serial.print("test device alarm state: ");
        Serial.println("OFF");
    }
    else
    {
        Serial.print("test device alarm state: ");
        Serial.println("ON");
    }
    Serial.println("--");
#endif
#endif // END OF TEST_DEVICE LOGIC

#ifdef ENABLE_WATER_PUMP
    checkWaterPumpAlarms();
    // TODO: check if device repeat toggle is enabled.
    // TODO: if continuousToggle is enabled in settings, run toggle
    // toggleWaterPump();

    // TODO: if onetimeInterval is enabled in settings run code below
    // TODO: turn the code below into a function
    if (waterPumpTimer.justFinished())
    {
        waterPump.turnDeviceOff();
    }

#ifdef ENABLE_SERIAL_OUTPUT
    serial_displayWaterPumpState();

    if (waterPump.alarmState == HIGH)
    {
        Serial.print("Water Pump alarm state: ");
        Serial.println("OFF");
    }
    else
    {
        Serial.print("Water Pump alarm state: ");
        Serial.println("ON");
    }
    Serial.println("--");
#endif // END OF SERIAL_OUTPUT

#endif // END OF WATER_PUMP LOGIC

#ifdef ENABLE_OXYGEN_PUMP
    checkOxygenPumpAlarms();
    // TODO: check if device off timer is enabled.
    // TODO: check if device repeat toggle is enabled.
    //  toggleOxygenPump();

#ifdef ENABLE_SERIAL_OUTPUT
    serial_displayOxygenPumpState();

    if (oxygenPump.alarmState == HIGH)
    {
        Serial.print("Oxygen Pump alarm state: ");
        Serial.println("OFF");
    }
    else
    {
        Serial.print("Oxygen Pump alarm state: ");
        Serial.println("ON");
    }
    Serial.println("--");
#endif //

#endif // END OF OCYGEN_PUMP LOGIC

#ifdef ENABLE_WHITE_LIGHT
    checkWhiteLightAlarms();
    // TODO: check if device off timer is enabled.
    // TODO: check if device repeat toggle is enabled.
    //  toggleWhiteLight();

#ifdef ENABLE_SERIAL_OUTPUT
    serial_displayWhiteLightState();

    if (whiteLight.alarmState == HIGH)
    {
        Serial.print("White Light alarm state: ");
        Serial.println("OFF");
    }
    else
    {
        Serial.print("White Light alarm state: ");
        Serial.println("ON");
    }
    Serial.println("--");
#endif //

#endif // END OF WHITE_LIGHT LOGIC

#ifdef ENABLE_RED_LIGHT
    checkRedLightAlarms();
    // TODO: check if device off timer is enabled.
    // TODO: check if device repeat toggle is enabled.
    //  toggleRedLight();

#ifdef ENABLE_SERIAL_OUTPUT
    serial_displayRedLightState();

    if (redLight.alarmState == HIGH)
    {
        Serial.print("Red Light alarm state: ");
        Serial.println("OFF");
    }
    else
    {
        Serial.print("Red Light alarm state: ");
        Serial.println("ON");
    }
    Serial.println("--");
#endif //

#endif // END OF RED_LIGHT LOGIC

#pragma endregion

#ifdef ENABLE_SERIAL_OUTPUT // serial output footer
    Serial.println("");
    Serial.println("");
#endif

#ifdef ENABLE_OLED_OUTPUT          // if oled output is enabled in the config
    display.clearDisplay();        // clear the display contents
    oled_displayInfo_cycleState(); // cycle the state of the oled to show next device info
    oled_displayAllDeviceInfo();   // display the device info corresponding to the current state
    oled_displayTime(now, 0, 25);  // display the current time to the oled

    display.display(); // blast current frame to the display
#endif

    // TODO: add non-blocking timer here instead of delay
    delay(SYSTEM_UPDATE_INTERVAL); // delay to run at a steady rate
}

#pragma endregion

#pragma region STANDARD_FUNCTIONS
void setupSerialConnection(int baud, int startupDelay)
{
#ifdef ENABLE_SERIAL_OUTPUT // only output to the serial console if enabled
    Serial.begin(baud);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif
    Serial.println("Serial Connection Established.");
    delay(startupDelay);
#endif
}

#pragma endregion

#pragma region HID DEVICE LOGIC
//
void checkSafetySwitch()
{
    if (safetySwitch.getCurrentState() == LOW) // backwards logic
    {
        Serial.println("Safety Switch has been ACTIVATED!");
        testDevice.turnDeviceOff();
    }
}

#pragma endregion

#pragma region RELAY_LOGIC
// CHECK TEST DEVICE ALARMS
// Check the set alarms and see if any of them are triggered
void checkTestDeviceAlarms()
{

    // TEST DEVICE ALARM #1
    if (testDevice.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == TEST_DEVICE_ON_HOUR_A1) &&     // hour the device should turn on
            (now.minute() == TEST_DEVICE_ON_MINUTE_A1) && // minute the device should turn on
            (testDevice.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #1 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDeviceTimer.start(TEST_DEVICE_INTERVAL_A1); // start ON interval timer for the first time
            testDevice.turnDeviceOn();                      // turn ON the device for the first time
            testDevice.alarmState = LOW;                    // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == TEST_DEVICE_OFF_MINUTE_A1) && // minute the device should turn on
            (testDevice.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #1 reset!");
            Serial.println("Turning 'Test Device' OFF.");
#endif
            testDeviceTimer.stop();            // stop the ON interval timer
            testDevice.turnDeviceOff();        // turn OFF the device indefenitley
            testDevice.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            testDevice.lastAlarmTriggered = 1; // let program know this was the last alarm triggered so we dont run it again prematurily
            testDevice.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #1

    // TEST DEVICE ALARM #2
    if (testDevice.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == TEST_DEVICE_ON_HOUR_A2) &&     // hour the device should turn on
            (now.minute() == TEST_DEVICE_ON_MINUTE_A2) && // minute the device should turn on
            (testDevice.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #2 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDevice.turnDeviceOn();                      // turn ON the device for the first time
            testDeviceTimer.start(TEST_DEVICE_INTERVAL_A2); // start ON interval timer for the first time
            testDevice.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() == TEST_DEVICE_OFF_MINUTE_A2) && // minute the device should turn on
            (testDevice.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #2 reset!");
            Serial.println("Turning 'Test Device' OFF.");
#endif
            testDevice.turnDeviceOff();        // turn OFF the device indefenitley
            testDeviceTimer.stop();            // stop the ON interval timer
            testDevice.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            testDevice.lastAlarmTriggered = 2; // let program know this was the last alarm triggered so we dont run it again prematurily
            testDevice.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #2

    // TEST DEVICE ALARM #3
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == TEST_DEVICE_ON_HOUR_A3) &&     // hour the device should turn on
            (now.minute() == TEST_DEVICE_ON_MINUTE_A3) && // minute the device should turn on
            (testDevice.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #3 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDevice.turnDeviceOn();                      // turn ON the device for the first time
            testDeviceTimer.start(TEST_DEVICE_INTERVAL_A3); // start ON interval timer for the first time
            testDevice.alarmState = LOW;                    // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() == TEST_DEVICE_OFF_MINUTE_A3) && // minute the device should turn on
            (testDevice.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #3 reset!");
            Serial.println("Turning 'Test Device' OFF.");
#endif
            testDevice.turnDeviceOff();        // turn OFF the device indefenitley
            testDeviceTimer.stop();            // stop the ON interval timer
            testDevice.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            testDevice.lastAlarmTriggered = 3; // let program know this was the last alarm triggered so we dont run it again prematurily
            testDevice.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #3
}
// Toggle the device state on/off forever
void toggleTestDevice()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (testDeviceTimer.justFinished())
    {                             // if the interval timer has finished
        testDeviceTimer.repeat(); // repeat the interval timer

        // testDevice.deviceState = !testDevice.deviceState; // toggle device current state

        // toggle device
        if (testDevice.deviceState == HIGH)
        {                              // if the device state is ON
            testDevice.turnDeviceOn(); // toggle the device ON
        }
        else
        {                               // else if the state is OFF
            testDevice.turnDeviceOff(); // toggle the device OFF
        }
    }
}

// CHECK WATER PUMP ALARMS
// Check the set alarms and see if any of them are triggered
void checkWaterPumpAlarms()
{
    // WATER PUMP ALARM #1
    if (waterPump.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WATER_PUMP_ON_HOUR_A1) &&     // hour the device should turn on
            (now.minute() == WATER_PUMP_ON_MINUTE_A1) && // minute the device should turn on
            (waterPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPumpTimer.start(WATER_PUMP_INTERVAL_A1); // start ON interval timer for the first time
            waterPump.turnDeviceOn();                     // turn ON the device for the first time
            waterPump.alarmState = LOW;                   // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == WATER_PUMP_OFF_MINUTE_A1) && // minute the device should turn on
            (waterPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #1 reset!");
            Serial.println("Turning 'Water Pump' OFF.");
#endif
            waterPumpTimer.stop();            // stop the ON interval timer
            waterPump.turnDeviceOff();        // turn OFF the device indefenitley
            waterPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            waterPump.lastAlarmTriggered = 1; // let program know this was the last alarm triggered so we dont run it again prematurily
            waterPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #1

    // WATER PUMP ALARM #2
    if (waterPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WATER_PUMP_ON_HOUR_A2) &&     // hour the device should turn on
            (now.minute() == WATER_PUMP_ON_MINUTE_A2) && // minute the device should turn on
            (waterPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPump.turnDeviceOn();                     // turn ON the device for the first time
            waterPumpTimer.start(WATER_PUMP_INTERVAL_A2); // start ON interval timer for the first time
            waterPump.alarmState = LOW;                   // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() == WATER_PUMP_OFF_MINUTE_A2) && // minute the device should turn on
            (waterPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #2 reset!");
            Serial.println("Turning 'Water Pump' OFF.");
#endif
            waterPump.turnDeviceOff();        // turn OFF the device indefenitley
            waterPumpTimer.stop();            // stop the ON interval timer
            waterPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            waterPump.lastAlarmTriggered = 2; // let program know this was the last alarm triggered so we dont run it again prematurily
            waterPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #2

    // WATER PUMP ALARM #3
    if (waterPump.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WATER_PUMP_ON_HOUR_A3) &&     // hour the device should turn on
            (now.minute() == WATER_PUMP_ON_MINUTE_A3) && // minute the device should turn on
            (waterPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPump.turnDeviceOn();                     // turn ON the device for the first time
            waterPumpTimer.start(WATER_PUMP_INTERVAL_A3); // start ON interval timer for the first time
            waterPump.alarmState = LOW;                   // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() == WATER_PUMP_OFF_MINUTE_A3) && // minute the device should turn on
            (waterPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #3 reset!");
            Serial.println("Turning 'Water Pump' OFF.");
#endif
            waterPump.turnDeviceOff();        // turn OFF the device indefenitley
            waterPumpTimer.stop();            // stop the ON interval timer
            waterPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            waterPump.lastAlarmTriggered = 3; // let program know this was the last alarm triggered so we dont run it again prematurily
            waterPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #3
}
// Toggle the device state on/off forever
void toggleWaterPump()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (waterPumpTimer.justFinished())
    {                            // if the interval timer has finished
        waterPumpTimer.repeat(); // repeat the interval timer

        waterPump.deviceState = !waterPump.deviceState; // toggle device current state

        // toggle device
        if (waterPump.deviceState)
        {                             // if the device state is ON
            waterPump.turnDeviceOn(); // toggle the device ON
        }
        else
        {                                  // else if the state is OFF
            waterPump.turnDeviceOff();     // toggle the device OFF
            waterPump.currentCycleCount++; // increase current cycle count
        }
    }
}

// CHECK OXYGEN PUMP ALARMS
//  Check the set alarms and see if any of them are triggered
void checkOxygenPumpAlarms()
{

    // OXYGEN PUMP ALARM #1
    if (oxygenPump.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_ON_HOUR_A1) &&     // hour the device should turn on
            (now.minute() == OXYGEN_PUMP_ON_MINUTE_A1) && // minute the device should turn on
            (oxygenPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL_A1); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn();                      // turn ON the device for the first time
            oxygenPump.alarmState = LOW;                    // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == OXYGEN_PUMP_OFF_MINUTE_A1) && // minute the device should turn on
            (oxygenPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #1 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPumpTimer.stop();            // stop the ON interval timer
            oxygenPump.turnDeviceOff();        // turn OFF the device indefenitley
            oxygenPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            oxygenPump.lastAlarmTriggered = 1; // let program know this was the last alarm triggered so we dont run it again prematurily
            oxygenPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #1

    // OXYGEN PUMP ALARM #2
    if (oxygenPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_ON_HOUR_A2) &&     // hour the device should turn on
            (now.minute() == OXYGEN_PUMP_ON_MINUTE_A2) && // minute the device should turn on
            (oxygenPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.turnDeviceOn();                      // turn ON the device for the first time
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL_A2); // start ON interval timer for the first time
            oxygenPump.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() == OXYGEN_PUMP_OFF_MINUTE_A2) && // minute the device should turn on
            (oxygenPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #2 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.turnDeviceOff();        // turn OFF the device indefenitley
            oxygenPumpTimer.stop();            // stop the ON interval timer
            oxygenPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            oxygenPump.lastAlarmTriggered = 2; // let program know this was the last alarm triggered so we dont run it again prematurily
            oxygenPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #2

    // OXYGEN PUMP ALARM #3
    if (oxygenPump.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_ON_HOUR_A3) &&     // hour the device should turn on
            (now.minute() == OXYGEN_PUMP_ON_MINUTE_A3) && // minute the device should turn on
            (oxygenPump.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.turnDeviceOn();                      // turn ON the device for the first time
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL_A3); // start ON interval timer for the first time
            oxygenPump.alarmState = LOW;                    // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == OXYGEN_PUMP_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() == OXYGEN_PUMP_OFF_MINUTE_A3) && // minute the device should turn on
            (oxygenPump.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #3 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.turnDeviceOff();        // turn OFF the device indefenitley
            oxygenPumpTimer.stop();            // stop the ON interval timer
            oxygenPump.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            oxygenPump.lastAlarmTriggered = 3; // let program know this was the last alarm triggered so we dont run it again prematurily
            oxygenPump.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #3
}
// Toggle the device state on/off forever
void toggleOxygenPump()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (oxygenPumpTimer.justFinished())
    {                             // if the interval timer has finished
        oxygenPumpTimer.repeat(); // repeat the interval timer

        oxygenPump.deviceState = !oxygenPump.deviceState; // toggle device current state

        // toggle device
        if (oxygenPump.deviceState)
        {                              // if the device state is ON
            oxygenPump.turnDeviceOn(); // toggle the device ON
        }
        else
        {                                   // else if the state is OFF
            oxygenPump.turnDeviceOff();     // toggle the device OFF
            oxygenPump.currentCycleCount++; // increase current cycle count
        }
    }
}

// CHECK WHITE LIGHT ALARMS
// Check the set alarms and see if any of them are triggered
void checkWhiteLightAlarms()
{

    // WHITE LIGHT ALARM #1
    if (whiteLight.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WHITE_LIGHT_ON_HOUR_A1) &&     // hour the device should turn on
            (now.minute() == WHITE_LIGHT_ON_MINUTE_A1) && // minute the device should turn on
            (whiteLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #1 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL_A1); // start ON interval timer for the first time
            whiteLight.turnDeviceOn();                      // turn ON the device for the first time
            whiteLight.alarmState = LOW;                    // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WHITE_LIGHT_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == WHITE_LIGHT_OFF_MINUTE_A1) && // minute the device should turn on
            (whiteLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #1 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLightTimer.stop();            // stop the ON interval timer
            whiteLight.turnDeviceOff();        // turn OFF the device indefenitley
            whiteLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            whiteLight.lastAlarmTriggered = 1; // let program know this was the last alarm triggered so we dont run it again prematurily
            whiteLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #1

    // WHITE LIGHT ALARM #2
    if (whiteLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WHITE_LIGHT_ON_HOUR_A2) &&     // hour the device should turn on
            (now.minute() == WHITE_LIGHT_ON_MINUTE_A2) && // minute the device should turn on
            (whiteLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #2 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.turnDeviceOn();                      // turn ON the device for the first time
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL_A2); // start ON interval timer for the first time
            whiteLight.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WHITE_LIGHT_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() == WHITE_LIGHT_OFF_MINUTE_A2) && // minute the device should turn on
            (whiteLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #2 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.turnDeviceOff();        // turn OFF the device indefenitley
            whiteLightTimer.stop();            // stop the ON interval timer
            whiteLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            whiteLight.lastAlarmTriggered = 2; // let program know this was the last alarm triggered so we dont run it again prematurily
            whiteLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #2

    // WHITE LIGHT ALARM #3
    if (whiteLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WHITE_LIGHT_ON_HOUR_A3) &&     // hour the device should turn on
            (now.minute() == WHITE_LIGHT_ON_MINUTE_A3) && // minute the device should turn on
            (whiteLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #3 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.turnDeviceOn();                      // turn ON the device for the first time
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL_A3); // start ON interval timer for the first time
            whiteLight.alarmState = LOW;                    // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WHITE_LIGHT_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() == WHITE_LIGHT_OFF_MINUTE_A3) && // minute the device should turn on
            (whiteLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #3 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.turnDeviceOff();        // turn OFF the device indefenitley
            whiteLightTimer.stop();            // stop the ON interval timer
            whiteLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            whiteLight.lastAlarmTriggered = 3; // let program know this was the last alarm triggered so we dont run it again prematurily
            whiteLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #3

    // WHITE LIGHT ALARM #4
    if (whiteLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == WHITE_LIGHT_ON_HOUR_A4) &&     // hour the device should turn on
            (now.minute() == WHITE_LIGHT_ON_MINUTE_A4) && // minute the device should turn on
            (whiteLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #4 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.turnDeviceOn();                      // turn ON the device for the first time
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL_A4); // start ON interval timer for the first time
            whiteLight.alarmState = LOW;                    // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WHITE_LIGHT_OFF_HOUR_A4) &&     // if the current time matches the set alarm time
            (now.minute() == WHITE_LIGHT_OFF_MINUTE_A4) && // minute the device should turn on
            (whiteLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #4 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.turnDeviceOff();        // turn OFF the device indefenitley
            whiteLightTimer.stop();            // stop the ON interval timer
            whiteLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            whiteLight.lastAlarmTriggered = 4; // let program know this was the last alarm triggered so we dont run it again prematurily
            whiteLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #4
}
// Toggle the device state on/off forever
void toggleWhiteLight()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (whiteLightTimer.justFinished())
    {                             // if the interval timer has finished
        whiteLightTimer.repeat(); // repeat the interval timer

        whiteLight.deviceState = !whiteLight.deviceState; // toggle device current state

        // toggle device
        if (whiteLight.deviceState)
        {                              // if the device state is ON
            whiteLight.turnDeviceOn(); // toggle the device ON
        }
        else
        {                                   // else if the state is OFF
            whiteLight.turnDeviceOff();     // toggle the device OFF
            whiteLight.currentCycleCount++; // increase current cycle count
        }
    }
}

// CHECK RED LIGHT ALARMS
// Check the set alarms and see if any of them are triggered
void checkRedLightAlarms()
{

    // RED LIGHT ALARM #1
    if (redLight.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == RED_LIGHT_ON_HOUR_A1) &&     // hour the device should turn on
            (now.minute() == RED_LIGHT_ON_MINUTE_A1) && // minute the device should turn on
            (redLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #1 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLightTimer.start(RED_LIGHT_INTERVAL_A1); // start ON interval timer for the first time
            redLight.turnDeviceOn();                    // turn ON the device for the first time
            redLight.alarmState = LOW;                  // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == RED_LIGHT_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == RED_LIGHT_OFF_MINUTE_A1) && // minute the device should turn on
            (redLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #1 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLightTimer.stop();            // stop the ON interval timer
            redLight.turnDeviceOff();        // turn OFF the device indefenitley
            redLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            redLight.lastAlarmTriggered = 1; // let program know this was the last alarm triggered so we dont run it again prematurily
            redLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #1

    // RED LIGHT ALARM #2
    if (redLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == RED_LIGHT_ON_HOUR_A2) &&     // hour the device should turn on
            (now.minute() == RED_LIGHT_ON_MINUTE_A2) && // minute the device should turn on
            (redLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #2 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.turnDeviceOn();                    // turn ON the device for the first time
            redLightTimer.start(RED_LIGHT_INTERVAL_A2); // start ON interval timer for the first time
            redLight.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == RED_LIGHT_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() == RED_LIGHT_OFF_MINUTE_A2) && // minute the device should turn on
            (redLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #2 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.turnDeviceOff();        // turn OFF the device indefenitley
            redLightTimer.stop();            // stop the ON interval timer
            redLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            redLight.lastAlarmTriggered = 2; // let program know this was the last alarm triggered so we dont run it again prematurily
            redLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #2

    // RED LIGHT ALARM #3
    if (redLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == RED_LIGHT_ON_HOUR_A3) &&     // hour the device should turn on
            (now.minute() == RED_LIGHT_ON_MINUTE_A3) && // minute the device should turn on
            (redLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #3 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.turnDeviceOn();                    // turn ON the device for the first time
            redLightTimer.start(RED_LIGHT_INTERVAL_A3); // start ON interval timer for the first time
            redLight.alarmState = LOW;                  // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == RED_LIGHT_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() == RED_LIGHT_OFF_MINUTE_A3) && // minute the device should turn on
            (redLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #3 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.turnDeviceOff();        // turn OFF the device indefenitley
            redLightTimer.stop();            // stop the ON interval timer
            redLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            redLight.lastAlarmTriggered = 3; // let program know this was the last alarm triggered so we dont run it again prematurily
            redLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #3

    // RED LIGHT ALARM #4
    if (redLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() == RED_LIGHT_ON_HOUR_A4) &&     // hour the device should turn on
            (now.minute() == RED_LIGHT_ON_MINUTE_A4) && // minute the device should turn on
            (redLight.alarmState == HIGH))              // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #4 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.turnDeviceOn();                    // turn ON the device for the first time
            redLightTimer.start(RED_LIGHT_INTERVAL_A4); // start ON interval timer for the first time
            redLight.alarmState = LOW;                  // set the state of the alarm to ON  to prevent double trigger
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == RED_LIGHT_OFF_HOUR_A4) &&     // if the current time matches the set alarm time
            (now.minute() == RED_LIGHT_OFF_MINUTE_A4) && // minute the device should turn on
            (redLight.alarmState == LOW))                // only run the code if the previous alarm was active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #4 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.turnDeviceOff();        // turn OFF the device indefenitley
            redLightTimer.stop();            // stop the ON interval timer
            redLight.alarmState = HIGH;      // reset the alarm state so another alarm is free to run
            redLight.lastAlarmTriggered = 4; // let program know this was the last alarm triggered so we dont run it again prematurily
            redLight.currentCycleCount = 0;  // reset the amount of cycle counts durring alarm
        }
    } // END ALARM #4
}
// Toggle the device state on/off forever
void toggleRedLight()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (redLightTimer.justFinished())
    {                           // if the interval timer has finished
        redLightTimer.repeat(); // repeat the interval timer

        redLight.deviceState = !redLight.deviceState; // toggle device current state

        // toggle device
        if (redLight.deviceState)
        {                            // if the device state is ON
            redLight.turnDeviceOn(); // toggle the device ON
        }
        else
        {                                 // else if the state is OFF
            redLight.turnDeviceOff();     // toggle the device OFF
            redLight.currentCycleCount++; // increase current cycle count
        }
    }
}

#pragma endregion

#pragma region SERIAL_OUTPUTS
// print current time from RTC to the serial console
void serial_dispalyCurrentTime()
{
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.println(now.second());
}
// print test device status to the serial console
void serial_displayTestDeviceState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((testDevice.alarmState == LOW)) // only print this if the alarm is triggered
    {
        if ((testDevice.deviceState == HIGH)) // if the device state is OFF
        {
            Serial.println("Test Device currently OFF.");
        }
        if ((testDevice.deviceState == LOW)) // if the device state is ON
        {
            Serial.println("Test Device currently ON.");
            Serial.print("Test Device current cyle count: ");
            Serial.println(testDevice.currentCycleCount);

            Serial.print("Test Device interval remaining: ");
            Serial.println(testDeviceTimer.remaining());
        }
    }
#endif
}
// print Water Pump status to the serial console
void serial_displayWaterPumpState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((waterPump.alarmState == LOW)) // only print this if the alarm is triggered
    {
        if ((waterPump.deviceState == HIGH)) // if the device state is OFF
        {
            Serial.println("Water Pump currently OFF.");
        }
        if ((waterPump.deviceState == LOW)) // if the device state is ON
        {
            Serial.println("Water Pump currently ON.");
            Serial.print("Water Pump current cyle count: ");
            Serial.println(waterPump.currentCycleCount);

            Serial.print("Water Pump interval remaining: ");
            Serial.println(waterPumpTimer.remaining());
        }
    }
#endif
}
// print Oxygen Pump status to the serial console
void serial_displayOxygenPumpState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((oxygenPump.alarmState == LOW)) // only print this if the alarm is triggered
    {
        if ((oxygenPump.deviceState == HIGH)) // if the device state is OFF
        {
            Serial.println("Oxygen Pump currently OFF.");
        }
        if ((oxygenPump.deviceState == LOW)) // if the device state is ON
        {
            Serial.println("Oxygen Pump currently ON.");
            Serial.print("Oxygen Pump current cyle count: ");
            Serial.println(oxygenPump.currentCycleCount);

            Serial.print("Oxygen Pump interval remaining: ");
            Serial.println(oxygenPumpTimer.remaining());
        }
    }
#endif
}
// print White Light status to the serial console
void serial_displayWhiteLightState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((whiteLight.alarmState == LOW)) // only print this if the alarm is triggered
    {
        if ((whiteLight.deviceState == HIGH)) // if the device state is OFF
        {
            Serial.println("White Light currently OFF.");
        }
        if ((whiteLight.deviceState == LOW)) // if the device state is ON
        {
            Serial.println("White Light currently ON.");
            Serial.print("White Light current cyle count: ");
            Serial.println(whiteLight.currentCycleCount);

            Serial.print("White Light interval remaining: ");
            Serial.println(whiteLightTimer.remaining());
        }
    }
#endif
}
// print Red Light status to the serial console
void serial_displayRedLightState()
{
#ifdef ENABLE_SERIAL_OUTPUT
    if ((redLight.alarmState == LOW)) // only print this if the alarm is triggered
    {
        if ((redLight.deviceState == HIGH)) // if the device state is OFF
        {
            Serial.println("Red Light currently OFF.");
        }
        if ((redLight.deviceState == LOW)) // if the device state is ON
        {
            Serial.println("Red Light currently ON.");
            Serial.print("Red Light current cyle count: ");
            Serial.println(redLight.currentCycleCount);

            Serial.print("Red Light interval remaining: ");
            Serial.println(redLightTimer.remaining());
        }
    }
#endif
}

#pragma endregion

#pragma region I2C DEVICE SETUP
// SETUP OLED DISPLAY
void setupDisplay(int startupDelay)
{
#ifdef ENABLE_OLED_OUTPUT
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.display();
    delay(startupDelay);
    display.clearDisplay();
    Serial.println("OLED Setup Complete.");
#endif
}
// SETUP RTC CLOCK
void setupRTC(int startupDelay)
{
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }
    else
    {
        Serial.println("RTC Setup Complete.");
        delay(startupDelay);
    }

// When time needs to be re-set on a previously configured device, the
// following line sets the RTC to the date & time this sketch was compiled
#ifdef RESET_RTC_TIME
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC has been updated with a new Date and Time! Be Careful!");
#endif
}
#pragma endregion

#pragma region OLED OUTPUTS
// PRINT TIME
void oled_displayTime(DateTime now, char xPos, char yPos)
{
#ifdef ENABLE_OLED_OUTPUT
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(xPos, yPos);       // Start at top-left corner
    // display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // display.print(" ");
    display.print(now.hour(), DEC);
    display.print(":");
    display.print(now.minute(), DEC);
    display.print(":");
    display.print(now.second(), DEC);
#endif
}

// TEMPERATURE
void oled_displayTemperature(DateTime now, char xPos, char yPos)
{
#ifdef ENABLE_OLED_OUTPUT
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(xPos, yPos);       // Start at top-left corner
    // display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // display.print(" ");
    display.print(rtc.getTemperature(), DEC);
    display.print(":");
#endif
}

// output test device info to display
void oled_displayTestDeviceInfo()
{
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("TEST DEVICE");

    if (testDevice.deviceState == HIGH) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (testDevice.deviceState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (testDevice.alarmState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Active");
        // TODO: device current relay state
    }
    else
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Inactive");
    }

    // display interval time left
    display.setCursor(80, 25); // Start at top-left corner
    display.println(testDeviceTimer.remaining());

    // draw device current cycle count to the oled display
    display.setCursor(80, 8); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(testDevice.totalCycleCount);
}

// output water pump info to display
void oled_displayWaterPumpInfo()
{
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WATER PUMP");

    if (waterPump.deviceState == HIGH) //
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (waterPump.deviceState == LOW) // H
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (waterPump.alarmState == LOW) //
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Active");
        // TODO: device current relay state
    }
    else
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Inactive");
    }
    // display interval time left
    display.setCursor(80, 25); // Start at top-left corner
    display.println(waterPumpTimer.remaining());

    // draw device current cycle count to the oled display
    display.setCursor(80, 8); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(waterPump.totalCycleCount);
}

// output oxygen pump info to display
void oled_displayOxygenPumpInfo()
{
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("OXYGEN PUMP");

    if (oxygenPump.deviceState == HIGH) //
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (oxygenPump.deviceState == LOW) //
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (oxygenPump.alarmState == LOW) //
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Active");
        // TODO: device current relay state
    }
    else
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Inactive");
    }

    // display interval time left
    display.setCursor(80, 25); // Start at top-left corner
    display.println(oxygenPumpTimer.remaining());

    // draw device current cycle count to the oled display
    display.setCursor(80, 8); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(oxygenPump.totalCycleCount);
}

// output white light info to display
void oled_displayWhiteLightInfo()
{
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WHITE LIGHT");

    if (whiteLight.deviceState == HIGH) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (whiteLight.deviceState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (whiteLight.alarmState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Active");
        // TODO: device current relay state
    }
    else
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Inactive");
    }

    // display interval time left
    display.setCursor(80, 25); // Start at top-left corner
    display.println(whiteLightTimer.remaining());

    // draw device current cycle count to the oled display
    display.setCursor(80, 8); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(whiteLight.totalCycleCount);
}

// output red light info to display
void oled_displayRedLightInfo()
{
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("RED LIGHT");

    if (redLight.deviceState == HIGH) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (redLight.deviceState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (redLight.alarmState == LOW) // HIGH = TRUE = ON = 1
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Active");
        // TODO: device current relay state
    }
    else
    {
        display.setCursor(0, 8); // Start at top-left corner
        display.println("Inactive");
    }

    // display interval time left
    display.setCursor(80, 25); // Start at top-left corner
    display.println(redLightTimer.remaining());

    // draw device current cycle count to the oled display
    display.setCursor(80, 8); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(redLight.totalCycleCount);
}

// display device info according to the state set by the time manager
void oled_displayAllDeviceInfo()
{

    switch (timer_oledCurrentState)
    {
    case 1:
        oled_displayWaterPumpInfo();
        break;
    case 2:
        oled_displayOxygenPumpInfo();
        break;
    case 3:
        oled_displayWhiteLightInfo();
        break;
    case 4:
        oled_displayRedLightInfo();
        break;
    }
}

// cycles the state of the oled at the set time interval
void oled_displayInfo_cycleState()
{
    bool oled_isTimerRunning;

    if (timer_oledCurrentState == 0) // if timer is not already running
    {
        timer_oled_CycleAllDeviceInfo.start(OLED_CYCLE_INTERVAL); // start the timer
        timer_oledCurrentState = 1;                               // set the state to the first deive
    }

    if (timer_oled_CycleAllDeviceInfo.justFinished()) // if timer just finished
    {
        timer_oledCurrentState++; // increase timer state to next device

        if (timer_oledCurrentState > AMOUNT_OF_DEVICES) // if timer state is above amount of devices available
        {
            timer_oledCurrentState = 1; // reset timer state to first device
        }

        timer_oled_CycleAllDeviceInfo.repeat(); // restart the timer
    }
}
#pragma endregion

#pragma endregion