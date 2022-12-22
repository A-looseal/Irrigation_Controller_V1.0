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
#define ENABLE_TEST_DEVICE // use this line to enable/dissable use of the TEST DEVICE
#define ENABLE_WATER_PUMP  // use this line to enable/dissable use of the WATER PUMP
// #define ENABLE_OXYGEN_PUMP // use this line to enable/dissable use of the OXYGEN PUMP
// #define ENABLE_WHITE_LIGHT // use this line to enable/dissable use of the WHITE LIGHT
// #define ENABLE_RED_LIGHT   // use this line to enable/dissable use of the RED_LIGHT
#pragma endregion

#pragma region SYSTEM_DEFENITIONS
// #define RESET_RTC_TIME
#define ENABLE_SERIAL_OUTPUT // use this line to enable/dissable usage of the SERIAL CONSOLE
// #define ENABLE_OLED_OUTPUT   // use this line to enable/disable usage of the OLED device
#define BAUD_RATE 9600
#define STARTUP_DELAY 1000
#pragma endregion
#pragma region SYSTEM_INTERVAL TIMERS
millisDelay timer_oled_displayGeneralInfo;
millisDelay timer_oled_displayPumpInfo;
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
#pragma region DEVICE_INTERVAL TIMERS
const unsigned long OXYGEN_PUMP_INTERVAL = 10000; // in ms
const unsigned long WHITE_LIGHT_INTERVAL = 10000; // in ms
const unsigned long RED_LIGHT_INTERVAL = 10000;   // in ms
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
    testDevice.lastAlarmTriggered = 3;

    // turn on Water Pump alarms
    waterPump.enableAlarm_A1 = true;
    waterPump.enableAlarm_A2 = false;
    waterPump.enableAlarm_A2 = false;
    waterPump.alarmState = HIGH; // make sure the alarms are not triggered on startup
    waterPump.lastAlarmTriggered = 1;

    // turn on Oxygen Pump alarms
    oxygenPump.enableAlarm_A1 = true;
    oxygenPump.enableAlarm_A2 = true;
    oxygenPump.enableAlarm_A3 = true;
    oxygenPump.alarmState = HIGH; // make sure the alarms are not triggered on startup
    oxygenPump.lastAlarmTriggered = 3;

    // turn on White Light alarms
    whiteLight.enableAlarm_A1 = true;
    whiteLight.enableAlarm_A2 = true;
    whiteLight.enableAlarm_A3 = true;
    whiteLight.alarmState = HIGH; // make sure the alarms are not triggered on startup
    whiteLight.lastAlarmTriggered = 3;

    // turn on Red Light alarms
    redLight.enableAlarm_A1 = true;
    redLight.enableAlarm_A2 = true;
    redLight.enableAlarm_A3 = true;
    redLight.alarmState = HIGH; // make sure the alarms are not triggered on startup
    redLight.lastAlarmTriggered = 3;

    delay(5000);
}

void loop()
{
    display.clearDisplay();
    display.display();

    now = rtc.now(); // get the time from the RTC module
    // oled_displayTime(now);     // prints the current time from RTC to the serial console
    // printTemperature_RTC(now); // prints the current temperature reading from RTC module to the serial console
    serial_dispalyCurrentTime();
    // checkSafetyButton();

    // TODO: Check if other devices are turned on
    // TODO: turn off other devices

#ifdef ENABLE_TEST_DEVICE
    checkTestDeviceAlarms();
    toggleTestDevice();

#ifdef ENABLE_SERIAL_OUTPUT
#ifdef ENABLE_OLED_OUTPUT
    oled_displayTestDeviceInfo();
#endif
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
#endif

#ifdef ENABLE_WATER_PUMP
    checkWaterPumpAlarms();
    // toggleWaterPump();

#ifdef ENABLE_SERIAL_OUTPUT
#ifdef ENABLE_OLED_OUTPUT
    oled_displayWaterPumpInfo();
#endif
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
#endif

#endif



#ifdef ENABLE_SERIAL_OUTPUT
    Serial.println("----------------");
#endif
    // delay(1000);
}

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
void checkSafetyButton()
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
            (testDevice.alarmState == HIGH))           // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #1 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn();                   // turn ON the device for the first time
            testDevice.alarmState = LOW;                 // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == TEST_DEVICE_OFF_MINUTE_A1) && // minute the device should turn on
            (testDevice.alarmState == LOW))             // only run the code if the previous alarm was active
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
            (now.minute() >= TEST_DEVICE_ON_MINUTE_A2) && // minute the device should turn on
            (testDevice.alarmState == HIGH) &&            // only run code if no other alarm is currently active
            (testDevice.lastAlarmTriggered == 1))         // only run the code if this WAS NOT the last alarm triggered
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #2 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDevice.turnDeviceOn();    // turn ON the device for the first time
            testDeviceTimer.start(60000); // start ON interval timer for the first time
            testDevice.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() >= TEST_DEVICE_OFF_MINUTE_A2) && // minute the device should turn on
            (testDevice.alarmState == LOW) &&              // only run the code if the previous alarm was active
            (testDevice.lastAlarmTriggered == 1))          // only turn off if this alarm WAS THE LAST alarm triggered
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
            (now.minute() >= TEST_DEVICE_ON_MINUTE_A3) && // minute the device should turn on
            (testDevice.alarmState == HIGH) &&            // only run code if no other alarm is currently active
            (testDevice.lastAlarmTriggered == 2))         // only run the code if this WAS NOT the last alarm triggered
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Test Device Alarm #3 Triggered!");
            Serial.println("Turning 'Test Device' ON.");
#endif
            testDevice.turnDeviceOn();    // turn ON the device for the first time
            testDeviceTimer.start(60000); // start ON interval timer for the first time
            testDevice.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == TEST_DEVICE_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() >= TEST_DEVICE_OFF_MINUTE_A3) && // minute the device should turn on
            (testDevice.alarmState == LOW) &&              // only run the code if the previous alarm was active
            (testDevice.lastAlarmTriggered == 2))          // only turn off if this alarm WAS THE LAST alarm triggered
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
            (waterPump.alarmState == HIGH))            // only run code if no other alarm is currently active
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn();                  // turn ON the device for the first time
            waterPump.alarmState = LOW;                // set the alarm state to on since we have a sucecssfull trigger event
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A1) &&     // if the current time matches the set alarm time
            (now.minute() == WATER_PUMP_OFF_MINUTE_A1) && // minute the device should turn on
            (waterPump.alarmState == LOW))              // only run the code if the previous alarm was active
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
            (now.minute() >= WATER_PUMP_ON_MINUTE_A2) && // minute the device should turn on
            (waterPump.alarmState == HIGH) &&            // only run code if no other alarm is currently active
            (waterPump.lastAlarmTriggered == 1))         // only run the code if this WAS NOT the last alarm triggered
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPump.turnDeviceOn();    // turn ON the device for the first time
            waterPumpTimer.start(60000); // start ON interval timer for the first time
            waterPump.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A2) &&     // if the current time matches the set alarm time
            (now.minute() >= WATER_PUMP_OFF_MINUTE_A2) && // minute the device should turn on
            (waterPump.alarmState == LOW) &&              // only run the code if the previous alarm was active
            (waterPump.lastAlarmTriggered == 1))          // only turn off if this alarm WAS THE LAST alarm triggered
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
            (now.minute() >= WATER_PUMP_ON_MINUTE_A3) && // minute the device should turn on
            (waterPump.alarmState == HIGH) &&            // only run code if no other alarm is currently active
            (waterPump.lastAlarmTriggered == 2))         // only run the code if this WAS NOT the last alarm triggered
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Water Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");
#endif
            waterPump.turnDeviceOn();    // turn ON the device for the first time
            waterPumpTimer.start(60000); // start ON interval timer for the first time
            waterPump.alarmState = LOW;
        }

        // TURN DEVICE OFF IF IT IS NOT ALARM TIME
        if ((now.hour() == WATER_PUMP_OFF_HOUR_A3) &&     // if the current time matches the set alarm time
            (now.minute() >= WATER_PUMP_OFF_MINUTE_A3) && // minute the device should turn on
            (waterPump.alarmState == LOW) &&              // only run the code if the previous alarm was active
            (waterPump.lastAlarmTriggered == 2))          // only turn off if this alarm WAS THE LAST alarm triggered
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
        delay(10);
        if ((now.hour() >= OXYGEN_PUMP_ON_HOUR_A1) &&
            (now.hour() < OXYGEN_PUMP_OFF_HOUR_A1) &&
            (oxygenPump.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 1;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn();                   // turn ON the device for the first time
        }

        if ((now.hour() > OXYGEN_PUMP_OFF_HOUR_A1) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) &&        // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #1 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.alarmState = false;    // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop();           // stop the ON interval timer
            oxygenPump.turnDeviceOff();       // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // OXYGEN PUMP ALARM #2
    if (oxygenPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= OXYGEN_PUMP_ON_HOUR_A2) &&
            (now.hour() < OXYGEN_PUMP_OFF_HOUR_A2) &&
            (oxygenPump.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 2;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > OXYGEN_PUMP_OFF_HOUR_A2) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) &&        // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 2))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #2 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop();        // stop the ON interval timer
            oxygenPump.turnDeviceOff();    // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }

    // OXYGEN PUMP ALARM #3
    if (oxygenPump.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= OXYGEN_PUMP_ON_HOUR_A3) &&
            (now.hour() < OXYGEN_PUMP_OFF_HOUR_A3) &&
            (oxygenPump.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 3;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > OXYGEN_PUMP_OFF_HOUR_A3) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) &&        // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #3 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop();        // stop the ON interval timer
            oxygenPump.turnDeviceOff();    // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }

    // OXYGEN PUMP ALARM #4
    if (oxygenPump.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= OXYGEN_PUMP_ON_HOUR_A4) &&
            (now.hour() < OXYGEN_PUMP_OFF_HOUR_A4) &&
            (oxygenPump.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm #4 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");
#endif
            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 4;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > OXYGEN_PUMP_OFF_HOUR_A4) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) &&        // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");
#endif
            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop();        // stop the ON interval timer
            oxygenPump.turnDeviceOff();    // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }
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
        if ((now.hour() >= WHITE_LIGHT_ON_HOUR_A1) &&
            (now.hour() < WHITE_LIGHT_OFF_HOUR_A1) &&
            (whiteLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #1 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 1;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > WHITE_LIGHT_OFF_HOUR_A1) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #1 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.alarmState = false;    // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop();           // stop the ON interval timer
            whiteLight.turnDeviceOff();       // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // WHITE LIGHT ALARM #2
    if (whiteLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= WHITE_LIGHT_ON_HOUR_A2) &&
            (now.hour() < WHITE_LIGHT_OFF_HOUR_A2) &&
            (whiteLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #2 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 2;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > WHITE_LIGHT_OFF_HOUR_A2) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 2))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #2 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop();        // stop the ON interval timer
            whiteLight.turnDeviceOff();    // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }

    // WHITE LIGHT ALARM #3
    if (whiteLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= WHITE_LIGHT_ON_HOUR_A3) &&
            (now.hour() < WHITE_LIGHT_OFF_HOUR_A3) &&
            (whiteLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #3 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 3;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > WHITE_LIGHT_OFF_HOUR_A3) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #3 reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop();        // stop the ON interval timer
            whiteLight.turnDeviceOff();    // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }

    // WHITE LIGHT ALARM #4
    if (whiteLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= WHITE_LIGHT_ON_HOUR_A4) &&
            (now.hour() < WHITE_LIGHT_OFF_HOUR_A4) &&
            (whiteLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm #4 Triggered!");
            Serial.println("Turning 'White Light' ON.");
#endif
            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 4;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn();                   // turn ON the device for the first time
        }
        if ((now.hour() > WHITE_LIGHT_OFF_HOUR_A4) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("White Light Alarm reset!");
            Serial.println("Turning 'White Light' OFF.");
#endif
            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop();        // stop the ON interval timer
            whiteLight.turnDeviceOff();    // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }
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
//  Check the set alarms and see if any of them are triggered
void checkRedLightAlarms()
{
    // RED LIGHT ALARM #1
    if (redLight.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= RED_LIGHT_ON_HOUR_A1) &&
            (now.hour() < RED_LIGHT_OFF_HOUR_A1) &&
            (redLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #1 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 1;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn();                 // turn ON the device for the first time
        }
        if ((now.hour() > RED_LIGHT_OFF_HOUR_A1) && // if the current time matches the set alarm time
            (redLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #1 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.alarmState = false;    // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop();           // stop the ON interval timer
            redLight.turnDeviceOff();       // turn OFF the device indefenitley
            redLight.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // RED LIGHT ALARM #2
    if (redLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= RED_LIGHT_ON_HOUR_A2) &&
            (now.hour() < RED_LIGHT_OFF_HOUR_A2) &&
            (redLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #2 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 2;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn();                 // turn ON the device for the first time
        }
        if ((now.hour() > RED_LIGHT_OFF_HOUR_A2) && // if the current time matches the set alarm time
            (redLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 2))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #2 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop();        // stop the ON interval timer
            redLight.turnDeviceOff();    // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }

    // RED LIGHT ALARM #3
    if (redLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= RED_LIGHT_ON_HOUR_A3) &&
            (now.hour() < RED_LIGHT_OFF_HOUR_A3) &&
            (redLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #3 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 3;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn();                 // turn ON the device for the first time
        }
        if ((now.hour() > RED_LIGHT_OFF_HOUR_A3) && // if the current time matches the set alarm time
            (redLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #3 reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop();        // stop the ON interval timer
            redLight.turnDeviceOff();    // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }

    // RED LIGHT ALARM #4
    if (redLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= RED_LIGHT_ON_HOUR_A4) &&
            (now.hour() < RED_LIGHT_OFF_HOUR_A4) &&
            (redLight.alarmState == false))
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm #4 Triggered!");
            Serial.println("Turning 'Red Light' ON.");
#endif
            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 4;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn();                 // turn ON the device for the first time
        }
        if ((now.hour() > RED_LIGHT_OFF_HOUR_A4) && // if the current time matches the set alarm time
            (redLight.alarmState == true) &&        // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1))     // if the alarm triggered is this one
        {
#ifdef ENABLE_SERIAL_OUTPUT
            Serial.println("Red Light Alarm reset!");
            Serial.println("Turning 'Red Light' OFF.");
#endif
            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop();        // stop the ON interval timer
            redLight.turnDeviceOff();    // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }
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
#ifdef ENABLE_OLED_OUTPUT // only output to the oled display if enabled in the config
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("TEST DEVICE");

    if (testDevice.deviceState == LOW) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (testDevice.deviceState == HIGH) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (testDevice.alarmState == HIGH) // HIGH = TRUE = ON = 1
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

    // draw device current cycle count to the oled display
    display.setCursor(100, 25); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(testDevice.totalCycleCount);

    oled_displayTime(now, 0, 25);

    display.display();
#endif
}

// output water pump info to display
void oled_displayWaterPumpInfo()
{
#ifdef ENABLE_OLED_OUTPUT // only output to the oled display if enabled in the config
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WATER PUMP");

    if (waterPump.deviceState == LOW) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (waterPump.deviceState == HIGH) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (waterPump.alarmState == HIGH) // HIGH = TRUE = ON = 1
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

    // draw device current cycle count to the oled display
    display.setCursor(100, 25); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(waterPump.totalCycleCount);

    oled_displayTime(now, 0, 25);

    display.display();
#endif
}

// output oxygen pump info to display
void oled_displayOxygenPumpInfo()
{
#ifdef ENABLE_OLED_OUTPUT // only output to the oled display if enabled in the config
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("OXYGEN PUMP");

    if (oxygenPump.deviceState == LOW) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (oxygenPump.deviceState == HIGH) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (oxygenPump.alarmState == HIGH) // HIGH = TRUE = ON = 1
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

    // draw device current cycle count to the oled display
    display.setCursor(100, 25); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(oxygenPump.totalCycleCount);

    oled_displayTime(now, 0, 25);

    display.display();
#endif
}

// output white light info to display
void oled_displayWhiteLightInfo()
{
#ifdef ENABLE_OLED_OUTPUT // only output to the oled display if enabled in the config
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WHITE LIGHT");

    if (whiteLight.deviceState == LOW) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (whiteLight.deviceState == HIGH) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (whiteLight.alarmState == HIGH) // HIGH = TRUE = ON = 1
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

    // draw device current cycle count to the oled display
    display.setCursor(100, 25); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(whiteLight.totalCycleCount);

    oled_displayTime(now, 0, 25);

    display.display();
#endif
}

// output red light info to display
void oled_displayRedLightInfo()
{
#ifdef ENABLE_OLED_OUTPUT // only output to the oled display if enabled in the config
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("RED LIGHT");

    if (redLight.deviceState == LOW) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (redLight.deviceState == HIGH) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (redLight.alarmState == HIGH) // HIGH = TRUE = ON = 1
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

    // draw device current cycle count to the oled display
    display.setCursor(100, 25); // Start at top-left corner
    // display.print("Cycle: ");
    display.println(redLight.totalCycleCount);

    oled_displayTime(now, 0, 25);

    display.display();
#endif
}

#pragma endregion