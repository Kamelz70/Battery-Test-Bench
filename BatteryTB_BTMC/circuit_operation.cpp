#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "circuit_operation.h"
#include "circuit_ads.h"
#include "circuit_config.h"
#include "circuit_measurements.h"
Adafruit_ADS1115 ads;

/////////// Helper Functions
void updateControlAndPWM(float measuredVoltage, float voltage_diff);
void handleCharging();
void handleDischarging();
void handleOff();
bool updatePulseState(unsigned long currentMillis);

/////////////////

// === Control Parameters ===
bool flgDisChrg = 1; // 1 = Discharge, 0 = Charge
float currMaxChrgDisChrg = 1;
float minvoltagedischarg = 2.8;
float voltsChrgMax = 4.2;

float T_AntiWiUp = 4;
float pGainChrg = 0.1;
float pGainDisChrg = 0.1;
float iGainDicChrg = 0.4;
float iGainChrg = 0.4;
float T_LowPass = 1.6;
float resBat = 0.02;

// === Time Control ===
unsigned long previousMillis = 0;
unsigned long Gridtime = 5; // ms

// === Variables ===
float currDes;
float voltsShuntAbs;
float currLimChrgDisChrg;
float facPwmP_1;
float facPwmP_2;
float iGain_error;
float error;
float y = 0;
float Del_V = 0;
float derfacPwmAntiWiUp = 0;
float facPwmDeltaLim;
float facPwmLim;
float measuredVoltage;
float measuredCurrent;
float voltage_diff;

// for pulse discharging
unsigned long pulseOnTime = 100000000; // 120000;  // milliseconds ON
unsigned long pulseOffTime = 0;        // 60000;  // milliseconds OFF
bool pulseState = true;                // true = ON phase, false = OFF phase
unsigned long pulsePreviousMillis = 0;
//******************

void circuitOperationSetup()
{
  pinMode(PWM_PIN_CHARGE, OUTPUT);
  pinMode(SAFETY_RELAY_PIN, OUTPUT);
  pinMode(CHARGE_DISCHARGE_RELAY_PIN, OUTPUT); // relay output
  Serial.begin(115200);
  ledcAttachPin(PWM_PIN_CHARGE, 1); // Channel 1 for PWM charging
  ledcSetup(1, 20000, 10);          // 20 kHz, resolution 10-bit

  ledcAttachPin(PWM_PIN_DISCHARGE, 0); // Channel 0 for PWM discharging
  ledcSetup(0, 20000, 10);             // 20 kHz, resolution 10-bit

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1)
      ;
  }
  ads.setGain(GAIN_ONE); // ±4.096V range
}

void operateCircuit(enum CIRCUITMODE CircuitMode)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= Gridtime)
  {
    if (CircuitMode == CIRCUITOFF)
    {
      handleOff();
      return;
    }
    previousMillis += Gridtime;

    VoltageCurrentReading voltageCurrentS = getVoltageAndCurrent(CircuitMode);
    measuredVoltage = voltageCurrentS.voltage;
    measuredCurrent = voltageCurrentS.current;

    // Check pulse state
    if (updatePulseState(currentMillis))
    {
      // Only run this if in ON phase
      if (CircuitMode == CIRCUITCHARGING)
      {

        handleCharging();
        digitalWrite(CHARGE_DISCHARGE_RELAY_PIN, LOW); // Relay OFF during charging
      }
      else if (CircuitMode == CIRCUITDISCHARGING)
      {

        handleDischarging();
        digitalWrite(CHARGE_DISCHARGE_RELAY_PIN, HIGH); // Relay ON during discharging
      }
    }
    digitalWrite(SAFETY_RELAY_PIN, HIGH); // Relay ON during discharging
  }
}

void handleCharging()
{
  voltsShuntAbs = -1 * voltage_diff;      // corrected
  Del_V = voltsChrgMax - measuredVoltage; // corrected
  error = currDes - 4 * voltsShuntAbs;

  facPwmP_1 = pGainChrg * error;
  iGain_error = iGainChrg * error;

  updateControlAndPWM(measuredVoltage, voltage_diff);
}

void handleDischarging()
{
  voltsShuntAbs = voltage_diff; // this function is working perfectly
  Del_V = measuredVoltage - minvoltagedischarg;
  error = -1 * (currDes - 4 * voltsShuntAbs);

  facPwmP_1 = pGainDisChrg * error;
  iGain_error = iGainDicChrg * error;

  updateControlAndPWM(measuredVoltage, voltage_diff);
}
void handleOff()
{
  // TODO handle off
  digitalWrite(SAFETY_RELAY_PIN, LOW); // Relay ON during discharging
  Serial.println("Circuit off- cuttting safety relay");
}

void updateControlAndPWM(float measuredVoltage, float voltage_diff)
{

  // y += (1 / T_LowPass) * (Gridtime / 1000.0) * (Del_V - y);
  // currLimChrgDisChrg = max(0.0f, (1 / resBat) * y);
  y = Del_V * 10;
  currLimChrgDisChrg = max(-0.1f, y);
  currDes = min(currMaxChrgDisChrg, currLimChrgDisChrg);

  facPwmP_2 += (Gridtime / 1000.0) * (iGain_error - derfacPwmAntiWiUp);
  derfacPwmAntiWiUp = T_AntiWiUp * facPwmDeltaLim;
  facPwmDeltaLim = (facPwmP_1 + facPwmP_2) - facPwmLim;
  facPwmLim = constrain(facPwmP_1 + facPwmP_2, 0.0f, 1.0f);

  int pwmValue = constrain((int)(facPwmLim * 1023.0), 0, 1023);

  if (flgDisChrg < 0.5)
  {
    ledcWrite(1, pwmValue);
  }

  else
  {
    ledcWrite(0, pwmValue);
  }

  // First line: header
  Serial.println("Voltage | PWM | Measured Current | facPwmLim | Error | currDes | currLimChrgDisChrg | facPwmP_1 | y");

  // Second line: all values on one line
  Serial.print(measuredVoltage, 3);
  Serial.print(" | ");
  Serial.print(pwmValue);
  Serial.print("     |    ");
  Serial.print(measuredCurrent, 3);
  Serial.print("   |    ");
  Serial.print(facPwmLim, 3);
  Serial.print("    |    ");
  Serial.print(error, 3);
  Serial.print("   |   ");
  Serial.print(currDes, 3);
  Serial.print(" |    ");
  Serial.print(currLimChrgDisChrg, 3);
  Serial.print(" |    ");
  Serial.print(facPwmP_1, 3);
  Serial.print("   |    ");
  Serial.print(Del_V, 3);
  Serial.print(" |    ");
  Serial.print(y, 3); // Only use println here to end the full line
  Serial.print(voltage_diff, 3);
  Serial.print(" |    ");
  Serial.println(voltage_diff, 3);
}

bool updatePulseState(unsigned long currentMillis,enum CIRCUITMODE CircuitMode)
{
  if(CircuitMode==CIRCUITCHARGING)
  {
    if (pulseState && (currentMillis - pulsePreviousMillis >= pulseOnTime))
    {
      pulseState = false; // Switch to OFF
      pulsePreviousMillis = currentMillis;
      ledcWrite(1, 0);
      int pwmValue = 0; // Turn off PWM
    }
    else if (!pulseState && (currentMillis - pulsePreviousMillis >= pulseOffTime))
    {
      pulseState = true; // Switch to ON
      pulsePreviousMillis = currentMillis;
    }
  
    return pulseState;

  }
  else if (CircuitMode==CIRCUITCHARGING)
  {
    //TODO implement pulse discharging
    Serial.println("Pulse Discharging not yet implemented");
      return pulseState;
  }
}
//
float getCircuitVoltage()
{
  return measuredVoltage;
}
float getCircuitCurrent()
{
  return measuredCurrent;
}
void setPulseOnTime(unsigned long pulseOnTimeInput)
{
  unsigned long pulseOnTime = pulseOnTimeInput; 
}

void setPulseOffTime(unsigned long pulseOffTimeInput)
{
  unsigned long pulseOffTime = pulseOffTimeInput; 

}

