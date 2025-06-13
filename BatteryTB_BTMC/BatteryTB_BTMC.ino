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
  if (recievedString.length() == 1)
  {
    switch (recievedString[0])
    {
    case 'c':
      CircuitMode = CIRCUITCHARGING;
      break;
    case 'd':
      CircuitMode = CIRCUITDISCHARGING;
      break;
    case 'o':
      CircuitMode = CIRCUITOFF;
      break;
    }
  }
  else if (recievedString.equals("getsettings"))
  {
    String circuitSettings = getBoundariesString();
    sendBLEString(circuitSettings);
  }
}
void onBLEConnect()
{
  sendBLEString(getBoundarySettingsString());
  Serial.println("dispatchOnConnect");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // setup serial
  Serial.begin(115200);
  // Serial.begin(9600); // HC-05 default baud rate
  Serial.println("Serial Started");
  setupBLE(&onBLERecieved,&onBLEConnect);
  circuitOperationSetup();
}

void loop()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
delay(1000);
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - lastBLETriggerMillis >= BLE_SEND_INTERVAL)
  {
    lastBLETriggerMillis = currentMillis;

    String measurementsString = getRealtimeDataString(String((char)CircuitMode));
    sendBLEString(measurementsString);
  }
  
  // operateCircuit(CircuitMode);
}
