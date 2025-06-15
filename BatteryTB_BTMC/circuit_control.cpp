#include <StringSplitter.h>
#include "circuit_control.h"
#include "circuit_config.h"
#include "custom_types.h"

enum CIRCUITMODE CircuitMode = CIRCUITOFF;
int ChargingPulseOnTime = DEFAULT_PULSE_ON_TIME_S;
int ChargingPulseOffTime = DEFAULT_PULSE_OFF_TIME_S;
int ChargingCurrent = DEFAULT_CURRENT;
int DischargingPulseOnTime = DEFAULT_PULSE_ON_TIME_S;
int DischargingPulseOffTime = DEFAULT_PULSE_OFF_TIME_S;
int DischargingCurrent = DEFAULT_CURRENT;

void parseControlString(String controlString)
{

    if (controlString[0] == 'o' && controlString.length() == 1)
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
        ChargingPulseOnTime = splitter.getItemAtIndex(2).toFloat();
        ChargingPulseOffTime = splitter.getItemAtIndex(3).toFloat();
        Serial.println("ChargingCurrent:" + String(ChargingCurrent));
        Serial.println("ChargingPulseOnTime:" + String(ChargingPulseOnTime));
    }
    else if (controlString[0] == 'd')
    {
        CircuitMode = CIRCUITDISCHARGING;
        DischargingCurrent = splitter.getItemAtIndex(1).toFloat();
        DischargingPulseOnTime = splitter.getItemAtIndex(2).toFloat();
        DischargingPulseOffTime = splitter.getItemAtIndex(3).toFloat();
        Serial.println("DischargingCurrent:" + String(DischargingCurrent));
        Serial.println("DischargingPulseOnTime:" + String(DischargingPulseOnTime));
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
    return String(MAX_VOLTAGE, 2) + "," +    // 1
           String(MIN_VOLTAGE, 2) + "," +         // 2
           String(MAX_CURRENT) + "," +            // 3
           String(MIN_CURRENT, 2) + "," +         // 4
           String(MAX_TEMPERATURE, 2) + "," +     // 5
           String(MIN_TEMPERATURE, 2) + "," +     // 6
           String(MAX_PULSE_ON_TIME_S) + "," +    // 7
           String(MIN_PULSE_ON_TIME_S) + "," + // 8
           String(MAX_PULSE_OFF_TIME_S) + "," +    // 9
           String(MIN_PULSE_OFF_TIME_S) + "," + // 10
           String(DEFAULT_PULSE_ON_TIME_S) + "," +       // 11
           String(DEFAULT_PULSE_OFF_TIME_S) + "," +       // 12
           String(DEFAULT_VOLTAGE, 2) + "," +     // 13
           String(DEFAULT_CURRENT, 2);            // 14
}