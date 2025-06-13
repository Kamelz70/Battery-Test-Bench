#include <StringSplitter.h>
#include "bluetooth_controller.h"
#include "circuit_operation.h"
#include "circuit_control.h"
#include "circuit_measurements.h"
#include "custom_types.h"
#include "circuit_safety.h"
#define BLE_SEND_INTERVAL 1000
unsigned long lastBLETriggerMillis = 0;

enum CIRCUITMODE CircuitMode = CIRCUITOFF;
/////////////////////////////////////////////////////////callBack function to be sent to setup ble to be called on recieving data
void onBLERecieved(String recievedString)
{
  parseControlString(recievedString);
}
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
void onBLEConnect()
{
  Serial.println("dispatchOnConnect---------------");
  sendBLEString(getBoundarySettingsString());
  Serial.println("----------------------------------");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // setup serial
  Serial.begin(115200);
  // Serial.begin(9600); // HC-05 default baud rate
  Serial.println("Serial Started");
  setupBLE(&onBLERecieved, &onBLEConnect);
  circuitOperationSetup();
}

void loop()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(1000);
  unsigned long currentMillis = millis();
  if ((currentMillis - lastBLETriggerMillis >= BLE_SEND_INTERVAL) && isBLEConnected())
  {
    lastBLETriggerMillis = currentMillis;
    String measurementsString = getRealtimeDataString(String((char)CircuitMode));
    sendBLEString(measurementsString);
  }

  // operateCircuit(CircuitMode);
}
