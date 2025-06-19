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
  Serial.begin(9600);
  Serial.println("Serial Started");
  pinMode(LED_BUILTIN, OUTPUT);
  measurementSetup();
  circuitOperationSetup();
  setupBLE(&onBLERecieved, &onBLEConnect);
}

void loop()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  VoltageCurrentReading VoltageCurrentS = getVoltageAndCurrent(getCircuitMode());
  float Temperature = getSavedTemperature();
  if (Serial1.available())
  {
    updateTemperature();
  }
  // last is temp.
  CIRCUITSAFETYCODE safetyCode = checkCircuitSafety(VoltageCurrentS.current, VoltageCurrentS.voltage,Temperature);
  // if (safetyCode == ALL_SAFE)
  if (1)
  {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastBLETriggerMillis >= BLE_SEND_INTERVAL) && isBLEConnected())
    {
      lastBLETriggerMillis = currentMillis;
      String measurementsString = getRealtimeDataString(getCircuitMode());
      sendBLEString(measurementsString);
    }
  }
  else
  {
    setCircuitMode(CIRCUITOFF);
    String ErrorMsg = getSafetyCodeString(safetyCode);
    sendBLEString(ErrorMsg);
    Serial.print("UNSAFE SYSTEM STATE, KAPPUTTT!!!!");
  }
  operateCircuit(getCircuitMode());
}
