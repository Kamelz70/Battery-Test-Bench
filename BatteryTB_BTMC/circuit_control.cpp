#include <StringSplitter.h>
#include "circuit_control.h"
#include "circuit_operation.h"
#include "circuit_config.h"
#include "custom_types.h"

enum CIRCUITMODE CircuitMode = CIRCUITOFF;
unsigned long PulseOnTime = DEFAULT_PULSE_ON_TIME_S;
unsigned long PulseOffTime = DEFAULT_PULSE_OFF_TIME_S;
float Current = DEFAULT_CURRENT;

void parseControlString(String controlString)
{

    // if (controlString[0] == 'o' && controlString.length() == 1)
    if (controlString[0] == 'o')
    {
        CircuitMode = CIRCUITOFF;
        return;
    }

    StringSplitter splitter(controlString, ',', 3); // max 3 values
    if (splitter.getItemCount() != 3)
    {
        CircuitMode = CIRCUITOFF;
        Serial.println("more than 3 params recieved, switching off");
        return;
    }
    if (controlString[0] == 'c')
    {
        CircuitMode = CIRCUITCHARGING;
    }
    else if (controlString[0] == 'd')
    {
        CircuitMode = CIRCUITDISCHARGING;
    }
    Current = splitter.getItemAtIndex(1).toFloat();
    PulseOnTime = splitter.getItemAtIndex(2).toInt() * 1000;
    PulseOffTime = splitter.getItemAtIndex(3).toInt() * 1000;
    setPulseOnTime(PulseOnTime);
    // setPulseOffTime(PulseOffTime); //TODO Fix
    setDesiredCurrent(Current);
        
    Serial.println("PulseOffTime String:===============================");
    Serial.println(splitter.getItemAtIndex(3));
    Serial.println("Recieved PulseOffTime");
    Serial.println(PulseOffTime);
    Serial.println("Current:" + String(Current));
    Serial.println("PulseOnTime:" + String(PulseOnTime));
    Serial.println("PulseOffTime:" + String(PulseOffTime));
}

CIRCUITMODE getCircuitMode()
{
    return CircuitMode;
}
void setCircuitMode(enum CIRCUITMODE circuitModeInput)
{
    CircuitMode = circuitModeInput;
}

String getBoundarySettingsString()
{
    return String(MAX_VOLTAGE, 2) + "," +           // 1
           String(MIN_VOLTAGE, 2) + "," +           // 2
           String(MAX_CURRENT) + "," +              // 3
           String(MIN_CURRENT, 2) + "," +           // 4
           String(MAX_TEMPERATURE, 2) + "," +       // 5
           String(MIN_TEMPERATURE, 2) + "," +       // 6
           String(MAX_PULSE_ON_TIME_S) + "," +      // 7
           String(MIN_PULSE_ON_TIME_S) + "," +      // 8
           String(MAX_PULSE_OFF_TIME_S) + "," +     // 9
           String(MIN_PULSE_OFF_TIME_S) + "," +     // 10
           String(DEFAULT_PULSE_ON_TIME_S) + "," +  // 11
           String(DEFAULT_PULSE_OFF_TIME_S) + "," + // 12
           String(DEFAULT_VOLTAGE, 2) + "," +       // 13
           String(DEFAULT_CURRENT, 2);              // 14
}