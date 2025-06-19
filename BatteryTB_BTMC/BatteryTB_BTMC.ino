#include "bluetooth_controller.h"
#include "circuit_operation.h"
#include "circuit_control.h"
#include "circuit_measurements.h"
#include "custom_types.h"
#include "circuit_safety.h"
#include "circuit_config.h"
#define BLE_SEND_INTERVAL 1000
unsigned long lastBLETriggerMillis = 0;
unsigned long lastSerial1ActivityMillis = 0;
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
  CIRCUITMODE circuitMode = getCircuitMode();
  if (Serial1.available())
  {
    lastSerial1ActivityMillis = millis();
    updateTemperature();
  }
  else
  {
    if (millis() - lastSerial1ActivityMillis > SERIAL1_TIMEOUT)
    {
      setCircuitMode(CIRCUITOFF);
      sendBLEString("Normal NANO not connected, circuit turning off");
    }
  }
  // last is temp.
  CIRCUITSAFETYCODE safetyCode = checkCircuitSafety(VoltageCurrentS.current, VoltageCurrentS.voltage, Temperature, circuitMode);
  // if (1)
  if (safetyCode == ALL_SAFE)
  {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastBLETriggerMillis >= BLE_SEND_INTERVAL) && isBLEConnected())
    {
      lastBLETriggerMillis = currentMillis;
      String measurementsString = getRealtimeDataString(circuitMode);
      sendBLEString(measurementsString);
    }
  }
  else
  {
    triggerSafetyRelay();
    setCircuitMode(CIRCUITOFF);
    String ErrorMsg = getSafetyCodeString(safetyCode);
    Serial.print("Boundary Exceeded");
    sendBLEString(ErrorMsg);
  }
  operateCircuit(getCircuitMode());
}
