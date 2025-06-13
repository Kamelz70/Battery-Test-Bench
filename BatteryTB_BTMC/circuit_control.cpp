#include <StringSplitter.h>
#include "circuit_control.h"
#include "circuit_config.h"
#include "custom_types.h"

enum CIRCUITMODE CircuitMode = CIRCUITOFF;
int ChargingStopTime = DEFAULT_STOPTIME;
int ChargingCurrent = DEFAULT_CURRENT;
int DischargingStopTime = DEFAULT_STOPTIME;
int DischargingCurrent = DEFAULT_CURRENT;

void parseControlString(String controlString)
{

    //   switch (controlString[0])
    //   {
    //   case 'c':
    //     CircuitMode = CIRCUITCHARGING;
    //     break;
    //   case 'd':
    //     CircuitMode = CIRCUITDISCHARGING;
    //     break;
    //   case 'o':
    //     CircuitMode = CIRCUITOFF;
    //     return;
    //     break;
    //     default:
    //     return;
    //   }
    if (controlString[0] == 'o'&&controlString.length()==1)
    {
        CircuitMode = CIRCUITOFF;
        return;
    }

    StringSplitter splitter(controlString, ',', 3); // max 3 values
    if (splitter.getItemCount() != 3)
        return;

    if (controlString[0] == 'c')
    {
        CircuitMode = CIRCUITCHARGING;
        ChargingCurrent = splitter.getItemAtIndex(1).toFloat();
        ChargingStopTime = splitter.getItemAtIndex(2).toFloat();
        Serial.println("ChargingCurrent:" + String(ChargingCurrent));
        Serial.println("ChargingStopTime:" + String(ChargingStopTime));
    }
    else if (controlString[0] == 'd')
    {
        CircuitMode = CIRCUITDISCHARGING;
        DischargingCurrent = splitter.getItemAtIndex(1).toFloat();
        DischargingStopTime = splitter.getItemAtIndex(2).toFloat();
        Serial.println("DischargingCurrent:" + String(DischargingCurrent));
        Serial.println("DischargingStopTime:" + String(DischargingStopTime));
    }
    else
    {
        return;
    }
}

String getCircuitMode()
{
    return String((char)CircuitMode);
}
String getBoundarySettingsString()
{
    return String((int)MAX_VOLTAGE, 2) + "," + // 1
           String(MIN_VOLTAGE, 2) + "," +      // 2
           String(MAX_CURRENT) + "," +         // 3
           String(MIN_CURRENT, 2) + "," +      // 4
           String(MAX_TEMPERATURE, 2) + "," +  // 5
           String(MIN_TEMPERATURE, 2) + "," +  // 6
           String(MAX_STOPTIME) + "," +        // 7
           String(MIN_STOPTIME, 2) + "," +     // 8
           String(DEFAULT_STOPTIME) + "," +    // 9
           String(DEFAULT_VOLTAGE, 2) + "," +  // 10
           String(DEFAULT_CURRENT, 2);         // 11
}