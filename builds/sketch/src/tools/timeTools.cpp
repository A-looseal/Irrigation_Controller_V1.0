#line 1 "e:\\THE_CREATION_STATION\\#Arduino_Workspace\\Irrigation_Controller_V1.0\\src\\tools\\timeTools.cpp"
#include "timeTools.h"

unsigned long convertMinuteToMilliseconds(float minute){
    return minute * 60000;
}
