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

#pragma region ENABLE/DISABLE DEVICES

//#define ENABLE_WATER_PUMP  // use this line to enable/dissable use of the WATER PUMP
// #define ENABLE_OXYGEN_PUMP // use this line to enable/dissable use of the OXYGEN PUMP
// #define ENABLE_WHITE_LIGHT // use this line to enable/dissable use of the WHITE LIGHT
// #define ENABLE_RED_LIGHT      // use this line to enable/dissable use of the RED_LIGHT
#pragma endregion

#pragma region SYSTEM DEFENITIONS
// #define RESET_RTC_TIME

//#define ENABLE_OLED_OUTPUT // use this line to enable/disable usage of the OLED device


#pragma endregion
#pragma region SYSTEM INTERVAL TIMERS
millisDelay timer_oled_displayGeneralInfo;
millisDelay timer_oled_displayPumpInfo;
#pragma endregion

#pragma region DEVICE CONSTRUCTORS
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
#pragma region DEVICE INTERVAL TIMERS
const unsigned long TEST_DEVICE_INTERVAL = 10000; // in ms
const unsigned long WATER_PUMP_INTERVAL = 10000; // in ms
const unsigned long OXYGEN_PUMP_INTERVAL = 10000; // in ms
const unsigned long WHITE_LIGHT_INTERVAL = 10000; // in ms
const unsigned long RED_LIGHT_INTERVAL = 10000; // in ms
#pragma endregion

#pragma region HID CONSTRUCTORS
ButtonController safetySwitch(11);
ButtonController pumpOverride(10);
#pragma endregion

#pragma region I2C DEVICES




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



}

void loop()
{
    now = rtc.now(); // get the time from the RTC module
    // oled_displayTime(now);     // prints the current time from RTC to the serial console
    // printTemperature_RTC(now); // prints the current temperature reading from RTC module to the serial console

    //checkSafetyButton();


    checkTestDeviceAlarms();
    //toggleTestDevice();
    oled_displayTestDeviceInfo();
    printTestDeviceState();

    Serial.print("test device alarm state: ");
    Serial.println(testDevice.alarmState);
    Serial.println("----------------");
    Serial.println("----------------");
# 175 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
    delay(1000);
}

#pragma region STANDARD FUNCTIONS
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

#pragma region RELAY LOGIC
// Check the set alarms and see if any of them are triggered
void checkTestDeviceAlarms()
{
    // TEST DEVICE A1
    if (testDevice.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 1) &&
            (now.hour() < 10) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        else if ((testDevice.alarmState == true))
        {

            Serial.println("Test Device Alarm reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (testDevice.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() <= 11) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Test Device Alarm reset!");
            Serial.println("Turning 'Test Device' OFF.");

            testDevice.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            testDeviceTimer.stop(); // stop the ON interval timer
            testDevice.turnDeviceOff(); // turn OFF the device indefenitley
            testDevice.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() <= 17) &&
            (testDevice.alarmState == false))
        {

            Serial.println("Test Device Alarm Triggered!");
            Serial.println("Turning 'Test Device' ON.");

            testDevice.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
            testDevice.turnDeviceOn(); // turn ON the device for the first time
        }
        else
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

// Check the set alarms and see if any of them are triggered
void checkWaterPumpAlarms()
{
    // TEST DEVICE A1
    if (waterPump.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 1) &&
            (now.hour() <= 6) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Water Pump Alarm reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (waterPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() <= 11) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Water Pump Alarm reset!");
            Serial.println("Turning 'Water Pump' OFF.");

            waterPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            waterPumpTimer.stop(); // stop the ON interval timer
            waterPump.turnDeviceOff(); // turn OFF the device indefenitley
            waterPump.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A3
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() <= 17) &&
            (waterPump.alarmState == false))
        {

            Serial.println("Water Pump Alarm Triggered!");
            Serial.println("Turning 'Water Pump' ON.");

            waterPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            waterPumpTimer.start(WATER_PUMP_INTERVAL); // start ON interval timer for the first time
            waterPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
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

// Check the set alarms and see if any of them are triggered
void checkOxygenPumpAlarms()
{
    // TEST DEVICE A1
    if (oxygenPump.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 5) &&
            (now.hour() <= 6) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (waterPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() <= 11) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            oxygenPump.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            oxygenPumpTimer.stop(); // stop the ON interval timer
            oxygenPump.turnDeviceOff(); // turn OFF the device indefenitley
            oxygenPump.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A3
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() <= 17) &&
            (oxygenPump.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            oxygenPump.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            oxygenPumpTimer.start(OXYGEN_PUMP_INTERVAL); // start ON interval timer for the first time
            oxygenPump.turnDeviceOn(); // turn ON the device for the first time
        }
        else
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

// Check the set alarms and see if any of them are triggered
void checkWhiteLightAlarms()
{
    // TEST DEVICE A1
    if (whiteLight.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 5) &&
            (now.hour() <= 6) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (waterPump.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() <= 11) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

            whiteLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            whiteLightTimer.stop(); // stop the ON interval timer
            whiteLight.turnDeviceOff(); // turn OFF the device indefenitley
            whiteLight.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A3
    if (testDevice.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() <= 17) &&
            (whiteLight.alarmState == false))
        {

            Serial.println("Oxygen Pump Alarm Triggered!");
            Serial.println("Turning 'Oxygen Pump' ON.");

            whiteLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            whiteLightTimer.start(WHITE_LIGHT_INTERVAL); // start ON interval timer for the first time
            whiteLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Oxygen Pump Alarm reset!");
            Serial.println("Turning 'Oxygen Pump' OFF.");

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

// Check the set alarms and see if any of them are triggered
void checkRedLightAlarms()
{
    // TEST DEVICE A1
    if (redLight.enableAlarm_A1 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 5) &&
            (now.hour() <= 6) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            //redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Red Light Alarm reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A2
    if (redLight.enableAlarm_A2 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 10) &&
            (now.hour() <= 11) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
        {

            Serial.println("Red Light Alarm reset!");
            Serial.println("Turning 'Red Light' OFF.");

            redLight.alarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
            redLightTimer.stop(); // stop the ON interval timer
            redLight.turnDeviceOff(); // turn OFF the device indefenitley
            redLight.currentCycleCount = 0;
        }
    }
    // TEST DEVICE A3
    if (redLight.enableAlarm_A3 == true)
    {
        // TURN DEVICE ON IF IT IS ALARM TIME
        if ((now.hour() >= 16) &&
            (now.hour() <= 17) &&
            (redLight.alarmState == false))
        {

            Serial.println("Red Light Alarm Triggered!");
            Serial.println("Turning 'Red Light' ON.");

            redLight.alarmState = true; // set the alarm state to ON so we dont repeatidly run this code
            redLightTimer.start(RED_LIGHT_INTERVAL); // start ON interval timer for the first time
            redLight.turnDeviceOn(); // turn ON the device for the first time
        }
        else
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

#pragma region OUTPUTS
// print test device status to the serial console
void printTestDeviceState()
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
        }

        Serial.print("Test Device current cyle count: ");
        Serial.println(testDevice.currentCycleCount);

        Serial.print("Test Device interval remaining: ");
        Serial.println(testDeviceTimer.remaining());
    }

}
#pragma endregion

#pragma region I2C DEVICE SETUP
// SETUP OLED DISPLAY
void setupDisplay(int startupDelay)
{
# 819 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
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
# 861 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// TEMPERATURE
void oled_displayTemperature(DateTime now, char xPos, char yPos)
{
# 875 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// output test device info to display
void oled_displayTestDeviceInfo()
{
# 923 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// output water pump info to display
void oled_displayWaterPumpInfo()
{
# 971 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// output oxygen pump info to display
void oled_displayOxygenPumpInfo()
{
# 1019 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// output white light info to display
void oled_displayWhiteLightInfo()
{
# 1067 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

// output red light info to display
void oled_displayRedLightInfo()
{
# 1115 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
}

#pragma endregion
