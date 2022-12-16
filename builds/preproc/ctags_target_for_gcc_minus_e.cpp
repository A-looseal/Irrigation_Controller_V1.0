# 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino"
# 2 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2

# 4 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\Irrigation_Controller.ino" 2



const unsigned long TEST_DEVICE_INTERVAL_ON = 10000; // in ms
const unsigned long TEST_DEVICE_INTERVAL_OFF = 5000; // in ms

// TEST DEVICE
bool testDeviceAlarmState = 0x0;
millisDelay testDeviceTimer_OnInterval; // create a new interval timer object to track the on interval
millisDelay testDeviceTimer_OffInterval; // create a new interval timer object to track the off interval
DeviceController testDevice(13); // create a new device object

void setup()
{
    Serial.begin(9600);

    Serial.println("Enter R to start repeating or S to stop");
}

void loop()
{
    checkTestDeviceAlarms();

    // toggleTestDevice();

    printTestDeviceState();
    Serial.print("test device alarm state: ");
    Serial.println(testDeviceAlarmState);
    delay(1000);
}

// Check the set alarms and see if any of them are triggered
void checkTestDeviceAlarms()
{
    char c = getInput();

    // TURN DEVICE ON IF IT IS ALARM TIME
    if ((c == 'R') || (c == 'r') && (testDeviceAlarmState == false))
    {

        Serial.println("Test Device Alarm triggered!");
        Serial.println("Turning 'Test Device' ON.");

        testDeviceAlarmState = true; // set the alarm state to ON so we dont repeatidly run this code
        testDeviceTimer_OnInterval.start(TEST_DEVICE_INTERVAL_ON); // start ON interval timer for the first time
        testDeviceTimer_OffInterval.start(TEST_DEVICE_INTERVAL_OFF); // start OFF interval timer for the first time
        testDevice.turnDeviceOn(); // turn ON the device for the first time
    }

    // TURN DEVICE OFF IF IT IS NOT ALARM TIME
    if ((c == 'S') || (c == 's') && (testDeviceAlarmState == true))
    {


        Serial.println("Test Device Alarm reset!");
        Serial.println("Turning 'Test Device' OFF.");

        testDeviceAlarmState = false; // set the alarm state to OFF so we dont repeatidly run this code
        testDeviceTimer_OnInterval.stop(); // stop the ON interval timer
        testDeviceTimer_OffInterval.stop(); // stop the OFF interval timer
        testDevice.turnDeviceOff(); // turn OFF the device indefenitley
    }
}

// Toggle the device state on/off forever
void toggleTestDevice()
{ // if the OFF interval timer has finished, turn the device ON
    if (testDeviceTimer_OffInterval.justFinished())
    {
        if (testDevice.deviceCurrentState == 0x0)
        {
            testDevice.turnDeviceOn(); // turn the device ON
            testDeviceTimer_OffInterval.repeat(); // repeat the interval timer
        }
    }

    // if the ON interval timer has finished, turn the device OFF
    if (testDeviceTimer_OnInterval.justFinished())
    {
        if (testDevice.deviceCurrentState == 0x1)
        {
            testDevice.turnDeviceOff(); // turn the device OFF
            testDeviceTimer_OnInterval.repeat(); // repeat the interval timer
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

        Serial.print("Test Device ON interval remaining: ");
        Serial.println(testDeviceTimer_OnInterval.remaining());

        Serial.print("Test Device OFF interval remaining: ");
        Serial.println(testDeviceTimer_OffInterval.remaining());
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
