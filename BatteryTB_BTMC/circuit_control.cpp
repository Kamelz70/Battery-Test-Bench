#include "circuit_control.h"
#include "circuit_config.h"
#include "custom_types.h"

enum CIRCUITMODE CircuitMode = CIRCUITOFF;

void parseControlString(String controlString)
{
    
  switch (controlString[0])
  {
  case 'c':
    CircuitMode = CIRCUITCHARGING;
    break;
  case 'd':
    CircuitMode = CIRCUITDISCHARGING;
    break;
  case 'o':
    CircuitMode = CIRCUITOFF;
    return;
    break;
    default:
    return;
  }
  // (msg == "o") { CircuitMode = CIRCUITOFF; return; }

  // StringSplitter splitter(msg, ',', 3);   // max 3 tokens
  // if (splitter.getItemCount() != 3) return;

  // String modeStr = splitter.getItemAtIndex(0);
  // char   mode    = modeStr.charAt(0);
  // float  cur     = splitter.getItemAtIndex(1).toFloat();
  // int    stop    = splitter.getItemAtIndex(2).toInt();
}

String getCircuitMode()
{
    return String((char)CircuitMode);
}
String getBoundarySettingsString()
{
    return String((int)MAX_VOLTAGE, 2) + ","+ //1
    String(MIN_VOLTAGE, 2) + ","+             //2
    String(MAX_CURRENT) + ","+                //3
    String(MIN_CURRENT, 2) + ","+                //4
    String(MAX_TEMPERATURE, 2) + ","+            //5
    String(MIN_TEMPERATURE, 2) + ","+                //6
    String(MAX_STOPTIME) + ","+                //7
    String(MIN_STOPTIME, 2) + ","+                //8
    String(DEFAULT_STOPTIME) + ","+                //9
    String(DEFAULT_VOLTAGE, 2) + ","+                //10
    String(DEFAULT_CURRENT, 2);                //11
}