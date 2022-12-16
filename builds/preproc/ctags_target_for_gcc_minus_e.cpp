# 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
# 2 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2
# 3 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2

# 5 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2



const unsigned long TEST_DEVICE_INTERVAL = 10000; // in ms

// TEST DEVICE
bool testDeviceAlarmState = 0x0;
millisDelay testDeviceTimer; // create a new interval timer object to track the on interval
DeviceController testDevice(13); // create a new device object

ButtonController safetySwitch(10);
ButtonController pumpOverride(11);

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

        Serial.println("Test Device Alarm Triggered!");
        Serial.println("Turning 'Test Device' ON.");

        testDeviceAlarmState = true; // set the alarm state to ON so we dont repeatidly run this code
        testDeviceTimer.start(TEST_DEVICE_INTERVAL); // start ON interval timer for the first time
        testDevice.turnDeviceOn(); // turn ON the device for the first time
    }

    // TURN DEVICE OFF IF IT IS NOT ALARM TIME
    if ((c == 'S') || (c == 's') && (testDeviceAlarmState == true))
    {


        Serial.println("Test Device Alarm reset!");
        Serial.println("Turning 'Test Device' OFF.");

        testDeviceAlarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
        testDeviceTimer.stop(); // stop the ON interval timer
        testDevice.turnDeviceOff(); // turn OFF the device indefenitley
    }
}

// Toggle the device state on/off forever
void toggleTestDevice()
{ // if the OFF interval timer has finished, turn the device ON

    // this goes in the main loop
    if (testDeviceTimer.justFinished())
    { // if the interval timer has finished
        testDeviceTimer.repeat(); // repeat the interval timer
        Serial.println("ledDelay.repeat() called and led toggled");

        testDevice.deviceCurrentState = !testDevice.deviceCurrentState; // toggle device current state

        // toggle device
        if (testDevice.deviceCurrentState)
        { // if the device state is ON
            testDevice.turnDeviceOn(); // toggle the device ON
        }
        else
        { // else if the state is OFF
            testDevice.turnDeviceOff(); // toggle the device OFF
        }
    }
}

// print test device status to the serial console
void printTestDeviceState()
{

    if ((testDeviceAlarmState == 0x1)) // only print this if the alarm is triggered
    {
        if ((testDevice.deviceCurrentState == 0x0)) // if the device state is OFF
        {
            Serial.println("Test Device currently OFF.");
        }
        if ((testDevice.deviceCurrentState == 0x1)) // if the device state is ON
        {
            Serial.println("Test Device currently ON.");
        }

        Serial.print("Test Device current cyle count: ");
        Serial.println(testDevice.deviceCurrentCycleCount);

        Serial.print("Test Device interval remaining: ");
        Serial.println(testDeviceTimer.remaining());
    }

}

//
void checkSafetyButton()
{
    if (safetySwitch.getCurrentState() == 0x0) // backwards logic
    {
        Serial.println("Safety Switch has been turned ON");
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
