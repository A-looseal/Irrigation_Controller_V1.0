# 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
#pragma region INCLUDES
# 3 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 4 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 5 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 6 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 7 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
//
# 9 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 10 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 11 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2

# 13 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 14 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 15 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 16 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 17 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 18 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
#pragma endregion

#pragma region ENABLE /DISABLE_DEVICES
// #define ENABLE_TEST_DEVICE // use this line to enable/dissable use of the TEST DEVICE
// #define ENABLE_WATER_PUMP  // use this line to enable/dissable use of the WATER PUMP
// #define ENABLE_OXYGEN_PUMP // use this line to enable/dissable use of the OXYGEN PUMP
// #define ENABLE_WHITE_LIGHT // use this line to enable/dissable use of the WHITE LIGHT

#pragma endregion

#pragma region SYSTEM_DEFENITIONS
// #define RESET_RTC_TIME




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
millisDelay testDeviceTimer; // create a new interval timer object to track the on interval
DeviceController testDevice(13); // create a new device object
#pragma endregion
#pragma region DEVICE_INTERVAL TIMERS
const unsigned long TEST_DEVICE_INTERVAL = 10000; // in ms
const unsigned long WATER_PUMP_INTERVAL = 10000; // in ms
const unsigned long OXYGEN_PUMP_INTERVAL = 10000; // in ms
const unsigned long WHITE_LIGHT_INTERVAL = 10000; // in ms
const unsigned long RED_LIGHT_INTERVAL = 10000; // in ms
#pragma endregion

#pragma region HID_CONSTRUCTORS
ButtonController safetySwitch(11);
ButtonController pumpOverride(10);
#pragma endregion

#pragma region I2C_DEVICES




Adafruit_SSD1306 display(128 /* OLED display width, in pixels*/, 32 /* OLED display height, in pixels*/, &Wire, -1 /* Reset pin # (or -1 if sharing Arduino reset pin)*/);

RTC_DS3231 rtc;
DateTime now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#pragma endregion

void setup()
{
    setupSerialConnection(9600, 1000);
    // setup and establish a connection to the OLED DISPLAY
    setupDisplay(1000);
    setupRTC(1000);

    // turn on test device alarms
    testDevice.enableAlarm_A1 = true;
    testDevice.enableAlarm_A2 = true;
    testDevice.enableAlarm_A3 = true;
    testDevice.enableAlarm_A4 = true;

    // turn on Water Pump alarms
    waterPump.enableAlarm_A1 = true;
    waterPump.enableAlarm_A2 = true;
    waterPump.enableAlarm_A3 = true;
    waterPump.enableAlarm_A4 = true;

    // turn on Oxygen Pump alarms
    oxygenPump.enableAlarm_A1 = true;
    oxygenPump.enableAlarm_A2 = true;

    // turn on White Light alarms
    whiteLight.enableAlarm_A1 = true;
    whiteLight.enableAlarm_A2 = true;
    whiteLight.enableAlarm_A3 = true;

    // turn on Red Light alarms
    redLight.enableAlarm_A1 = true;
    redLight.enableAlarm_A2 = true;
    redLight.enableAlarm_A3 = true;
}

void loop()
{
    now = rtc.now(); // get the time from the RTC module
    // oled_displayTime(now);     // prints the current time from RTC to the serial console
    // printTemperature_RTC(now); // prints the current temperature reading from RTC module to the serial console

    // checkSafetyButton();
# 189 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
    checkRedLightAlarms();
    toggleRedLight();

    oled_displayRedLightInfo();


    serial_displayRedLightState();
    Serial.print("Red Light alarm state: ");
    Serial.println(redLight.alarmState);
    Serial.println("----------------");
    Serial.println("----------------");



    delay(1000);
}

#pragma region STANDARD_FUNCTIONS
void setupSerialConnection(int baud, int startupDelay)
{

    Serial.begin(baud);


    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB

    Serial.println("Serial Connection Established.");
    delay(startupDelay);

}

#pragma endregion

#pragma region HID DEVICE LOGIC
//
void checkSafetyButton()
{
    if (safetySwitch.getCurrentState() == 0x0) // backwards logic
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
        if ((now.hour() >= 1) &&
            (now.hour() < 9) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm #1 Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDevice.lastAlarmTriggered = 1;
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 9) && // if the current time matches the set alarm time
            (testDevice.alarmState == true) && // if the alarm is TRIGGERED
            (testDevice.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Test Device Alarm #1 reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // TEST DEVICE ALARM #2
    if (testDevice.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() < 11) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm #2 Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDevice.lastAlarmTriggered = 2;
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 11) && // if the current time matches the set alarm time
            (testDevice.alarmState == true) && // if the alarm is TRIGGERED
            (testDevice.lastAlarmTriggered == 2)) // if the alarm triggered is this one
        {

            Serial.println("Test Device Alarm #2 reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0;
        }
    }

    // TEST DEVICE ALARM #3
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() < 17) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm #3 Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDevice.lastAlarmTriggered = 3;
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 17) && // if the current time matches the set alarm time
            (testDevice.alarmState == true) && // if the alarm is TRIGGERED
            (testDevice.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Test Device Alarm #3 reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0;
        }
    }

    // TEST DEVICE ALARM #4
    if (testDevice.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 21) &&
            (now.hour() < 24) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm #4 Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDevice.lastAlarmTriggered = 4;
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 24) && // if the current time matches the set alarm time
            (testDevice.alarmState == true) && // if the alarm is TRIGGERED
            (testDevice.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Test Device Alarm reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0;
        }
    }
}
// Toggle the device state on/off forever
void toggleTestDevice()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (testDeviceTimer.justFinished())
    { // if the interval timer has finished
        testDeviceTimer.repeat(); // repeat the interval timer

        testDevice.deviceState = !testDevice.deviceState; // toggle device current state

        // toggle device
        if (testDevice.deviceState)
        { // if the device state is ON
            testDevice.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            testDevice.turnDeviceOff(); // toggle the device OFF
            testDevice.currentCycleCount++; // increase current cycle count
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
        if ((now.hour() >= 1) &&
            (now.hour() < 6) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPump.lastAlarmTriggered = 1;
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 6) && // if the current time matches the set alarm time
            (waterPump.alarmState == true) && // if the alarm is TRIGGERED
            (waterPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Water Pump Alarm #1 reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // WATER PUMP ALARM #2
    if (waterPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() < 11) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPump.lastAlarmTriggered = 2;
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 11) && // if the current time matches the set alarm time
            (waterPump.alarmState == true) && // if the alarm is TRIGGERED
            (waterPump.lastAlarmTriggered == 2)) // if the alarm triggered is this one
        {

            Serial.println("Water Pump Alarm #2 reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0;
        }
    }

    // WATER PUMP ALARM #3
    if (waterPump.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() < 17) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPump.lastAlarmTriggered = 3;
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 17) && // if the current time matches the set alarm time
            (waterPump.alarmState == true) && // if the alarm is TRIGGERED
            (waterPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Water Pump Alarm #3 reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0;
        }
    }

    // WATER PUMP ALARM #4
    if (waterPump.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 21) &&
            (now.hour() < 24) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm #4 Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPump.lastAlarmTriggered = 4;
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 24) && // if the current time matches the set alarm time
            (waterPump.alarmState == true) && // if the alarm is TRIGGERED
            (waterPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Water Pump Alarm reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0;
        }
    }
}
// Toggle the device state on/off forever
void toggleWaterPump()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (waterPumpTimer.justFinished())
    { // if the interval timer has finished
        waterPumpTimer.repeat(); // repeat the interval timer

        waterPump.deviceState = !waterPump.deviceState; // toggle device current state

        // toggle device
        if (waterPump.deviceState)
        { // if the device state is ON
            waterPump.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            waterPump.turnDeviceOff(); // toggle the device OFF
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
        if ((now.hour() >= 4) &&
            (now.hour() < 5) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm #1 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 1;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 5) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) && // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Oxygen Pump Alarm #1 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // OXYGEN PUMP ALARM #2
    if (oxygenPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 13) &&
            (now.hour() < 14) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm #2 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 2;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 14) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) && // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 2)) // if the alarm triggered is this one
        {

            Serial.println("Oxygen Pump Alarm #2 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }

    // OXYGEN PUMP ALARM #3
    if (oxygenPump.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 0) &&
            (now.hour() < 0) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm #3 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 3;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 0) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) && // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Oxygen Pump Alarm #3 reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }

    // OXYGEN PUMP ALARM #4
    if (oxygenPump.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 0) &&
            (now.hour() < 0) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm #4 Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPump.lastAlarmTriggered = 4;
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 0) && // if the current time matches the set alarm time
            (oxygenPump.alarmState == true) && // if the alarm is TRIGGERED
            (oxygenPump.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }
}
// Toggle the device state on/off forever
void toggleOxygenPump()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (oxygenPumpTimer.justFinished())
    { // if the interval timer has finished
        oxygenPumpTimer.repeat(); // repeat the interval timer

        oxygenPump.deviceState = !oxygenPump.deviceState; // toggle device current state

        // toggle device
        if (oxygenPump.deviceState)
        { // if the device state is ON
            oxygenPump.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            oxygenPump.turnDeviceOff(); // toggle the device OFF
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
        if ((now.hour() >= 6) &&
            (now.hour() < 9) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("White Light Alarm #1 Triggered!");
            Serial.println("Turning 'White Light' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 1;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 9) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) && // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("White Light Alarm #1 reset!");
            Serial.println("Turning 'White Light' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // WHITE LIGHT ALARM #2
    if (whiteLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() < 13) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("White Light Alarm #2 Triggered!");
            Serial.println("Turning 'White Light' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 2;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 13) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) && // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 2)) // if the alarm triggered is this one
        {

            Serial.println("White Light Alarm #2 reset!");
            Serial.println("Turning 'White Light' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }

    // WHITE LIGHT ALARM #3
    if (whiteLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 15) &&
            (now.hour() < 18) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("White Light Alarm #3 Triggered!");
            Serial.println("Turning 'White Light' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 3;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 18) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) && // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("White Light Alarm #3 reset!");
            Serial.println("Turning 'White Light' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }

    // WHITE LIGHT ALARM #4
    if (whiteLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 0) &&
            (now.hour() < 0) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("White Light Alarm #4 Triggered!");
            Serial.println("Turning 'White Light' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLight.lastAlarmTriggered = 4;
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 0) && // if the current time matches the set alarm time
            (whiteLight.alarmState == true) && // if the alarm is TRIGGERED
            (whiteLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("White Light Alarm reset!");
            Serial.println("Turning 'White Light' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }
}
// Toggle the device state on/off forever
void toggleWhiteLight()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (whiteLightTimer.justFinished())
    { // if the interval timer has finished
        whiteLightTimer.repeat(); // repeat the interval timer

        whiteLight.deviceState = !whiteLight.deviceState; // toggle device current state

        // toggle device
        if (whiteLight.deviceState)
        { // if the device state is ON
            whiteLight.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            whiteLight.turnDeviceOff(); // toggle the device OFF
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
        if ((now.hour() >= 5) &&
            (now.hour() < 6) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm #1 Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 1;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 6) && // if the current time matches the set alarm time
            (redLight.alarmState == true) && // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Red Light Alarm #1 reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0; // reset the amount of cycle counts durring alarm
        }
    }

    // RED LIGHT ALARM #2
    if (redLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() < 11) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm #2 Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 2;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 11) && // if the current time matches the set alarm time
            (redLight.alarmState == true) && // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 2)) // if the alarm triggered is this one
        {

            Serial.println("Red Light Alarm #2 reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }

    // RED LIGHT ALARM #3
    if (redLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() < 17) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm #3 Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 3;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 17) && // if the current time matches the set alarm time
            (redLight.alarmState == true) && // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Red Light Alarm #3 reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }

    // RED LIGHT ALARM #4
    if (redLight.enableAlarm_A4 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 21) &&
            (now.hour() < 22) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm #4 Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLight.lastAlarmTriggered = 4;
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        if ((now.hour() > 22) && // if the current time matches the set alarm time
            (redLight.alarmState == true) && // if the alarm is TRIGGERED
            (redLight.lastAlarmTriggered == 1)) // if the alarm triggered is this one
        {

            Serial.println("Red Light Alarm reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }
}
// Toggle the device state on/off forever
void toggleRedLight()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (redLightTimer.justFinished())
    { // if the interval timer has finished
        redLightTimer.repeat(); // repeat the interval timer

        redLight.deviceState = !redLight.deviceState; // toggle device current state

        // toggle device
        if (redLight.deviceState)
        { // if the device state is ON
            redLight.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            redLight.turnDeviceOff(); // toggle the device OFF
            redLight.currentCycleCount++; // increase current cycle count
        }
    }
}

#pragma endregion

#pragma region SERIAL_OUTPUTS
// print test device status to the serial console
void serial_displayTestDeviceState()
{

    if ((testDevice.alarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((testDevice.deviceState == 0x0)) // if the device state is OFF
        {
            Serial.println("Test Device currently OFF.");
        }
        if ((testDevice.deviceState == 0x1)) // if the device state is ON
        {
            Serial.println("Test Device currently ON.");
            Serial.print("Test Device current cyle count: ");
            Serial.println(testDevice.currentCycleCount);

            Serial.print("Test Device interval remaining: ");
            Serial.println(testDeviceTimer.remaining());
        }
    }

}
// print Water Pump status to the serial console
void serial_displayWaterPumpState()
{

    if ((waterPump.alarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((waterPump.deviceState == 0x0)) // if the device state is OFF
        {
            Serial.println("Water Pump currently OFF.");
        }
        if ((waterPump.deviceState == 0x1)) // if the device state is ON
        {
            Serial.println("Water Pump currently ON.");
            Serial.print("Water Pump current cyle count: ");
            Serial.println(waterPump.currentCycleCount);

            Serial.print("Water Pump interval remaining: ");
            Serial.println(waterPumpTimer.remaining());
        }
    }

}
// print Oxygen Pump status to the serial console
void serial_displayOxygenPumpState()
{

    if ((oxygenPump.alarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((oxygenPump.deviceState == 0x0)) // if the device state is OFF
        {
            Serial.println("Oxygen Pump currently OFF.");
        }
        if ((oxygenPump.deviceState == 0x1)) // if the device state is ON
        {
            Serial.println("Oxygen Pump currently ON.");
            Serial.print("Oxygen Pump current cyle count: ");
            Serial.println(oxygenPump.currentCycleCount);

            Serial.print("Oxygen Pump interval remaining: ");
            Serial.println(oxygenPumpTimer.remaining());
        }
    }

}
// print White Light status to the serial console
void serial_displayWhiteLightState()
{

    if ((whiteLight.alarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((whiteLight.deviceState == 0x0)) // if the device state is OFF
        {
            Serial.println("White Light currently OFF.");
        }
        if ((whiteLight.deviceState == 0x1)) // if the device state is ON
        {
            Serial.println("White Light currently ON.");
            Serial.print("White Light current cyle count: ");
            Serial.println(whiteLight.currentCycleCount);

            Serial.print("White Light interval remaining: ");
            Serial.println(whiteLightTimer.remaining());
        }
    }

}
// print Red Light status to the serial console
void serial_displayRedLightState()
{

    if ((redLight.alarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((redLight.deviceState == 0x0)) // if the device state is OFF
        {
            Serial.println("Red Light currently OFF.");
        }
        if ((redLight.deviceState == 0x1)) // if the device state is ON
        {
            Serial.println("Red Light currently ON.");
            Serial.print("Red Light current cyle count: ");
            Serial.println(redLight.currentCycleCount);

            Serial.print("Red Light interval remaining: ");
            Serial.println(redLightTimer.remaining());
        }
    }

}

#pragma endregion

#pragma region I2C DEVICE SETUP
// SETUP OLED DISPLAY
void setupDisplay(int startupDelay)
{

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(0x02 /*|< Gen. display voltage from 3.3V*/, 0x3C /*|< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32*/))
    {
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 1141 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 3
                      (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 1141 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
                      "SSD1306 allocation failed"
# 1141 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 3
                      ); &__c[0];}))
# 1141 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
                      )));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();
    Serial.println("OLED Setup Complete.");
    delay(startupDelay);

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




}
#pragma endregion

#pragma region OLED OUTPUTS
// PRINT TIME
void oled_displayTime(DateTime now, char xPos, char yPos)
{

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text
    display.setCursor(xPos, yPos); // Start at top-left corner
    // display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // display.print(" ");
    display.print(now.hour(), 10);
    display.print(":");
    display.print(now.minute(), 10);
    display.print(":");
    display.print(now.second(), 10);

}

// TEMPERATURE
void oled_displayTemperature(DateTime now, char xPos, char yPos)
{

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text
    display.setCursor(xPos, yPos); // Start at top-left corner
    // display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // display.print(" ");
    display.print(rtc.getTemperature(), 10);
    display.print(":");

}

// output test device info to display
void oled_displayTestDeviceInfo()
{

    display.clearDisplay();

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("TEST DEVICE");

    if (testDevice.deviceState == 0x0) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (testDevice.deviceState == 0x1) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (testDevice.alarmState == 0x1) // HIGH = TRUE = ON = 1
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

}

// output water pump info to display
void oled_displayWaterPumpInfo()
{

    display.clearDisplay();

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WATER PUMP");

    if (waterPump.deviceState == 0x0) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (waterPump.deviceState == 0x1) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (waterPump.alarmState == 0x1) // HIGH = TRUE = ON = 1
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

}

// output oxygen pump info to display
void oled_displayOxygenPumpInfo()
{

    display.clearDisplay();

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("OXYGEN PUMP");

    if (oxygenPump.deviceState == 0x0) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (oxygenPump.deviceState == 0x1) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (oxygenPump.alarmState == 0x1) // HIGH = TRUE = ON = 1
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

}

// output white light info to display
void oled_displayWhiteLightInfo()
{

    display.clearDisplay();

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("WHITE LIGHT");

    if (whiteLight.deviceState == 0x0) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (whiteLight.deviceState == 0x1) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (whiteLight.alarmState == 0x1) // HIGH = TRUE = ON = 1
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

}

// output red light info to display
void oled_displayRedLightInfo()
{

    display.clearDisplay();

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(1 /*|< Draw 'on' pixels*/); // Draw white text

    // draw current device name on the display
    display.setCursor(0, 0); // Start at top-left corner
    display.println("RED LIGHT");

    if (redLight.deviceState == 0x0) // LOW = FALSE = OFF = 0
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("OFF");
    }
    if (redLight.deviceState == 0x1) // HIGH = TRUE = ON = 1
    {
        display.setCursor(110, 0); // Start at top-left corner
        display.println("ON");
    }

    // draw device current alarm state on the display
    if (redLight.alarmState == 0x1) // HIGH = TRUE = ON = 1
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

}

#pragma endregion
