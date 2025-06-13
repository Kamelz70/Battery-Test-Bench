#include <StringSplitter.h>
#include "bluetooth_controller.h"
#include "circuit_operation.h"
#include "circuit_control.h"
#include "circuit_measurements.h"
#include "custom_types.h"
#include "circuit_safety.h"
#define BLE_SEND_INTERVAL 1000
unsigned long lastBLETriggerMillis = 0;

/////////////////////////////////////////////////////////callBack functions to be sent to setup ble
void onBLERecieved(String recievedString)
{
  parseControlString(recievedString);
}

void onBLEConnect()
{
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
  delay(100);
  unsigned long currentMillis = millis();
  if ((currentMillis - lastBLETriggerMillis >= BLE_SEND_INTERVAL) && isBLEConnected())
  {
    lastBLETriggerMillis = currentMillis;
    String measurementsString = getRealtimeDataString(getCircuitMode());
    sendBLEString(measurementsString);
  }

  // operateCircuit(CircuitMode);
}
