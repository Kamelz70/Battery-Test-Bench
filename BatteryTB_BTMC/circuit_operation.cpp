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
bool updatePulseState(unsigned long currentMillis, enum CIRCUITMODE CircuitModeInput);
template <typename T1, typename T2, typename T3, typename T4>
auto verifyValue(T1 value, T2 min, T3 max, T4 defaultValue) -> decltype(value + min + max + defaultValue);
/////////////////

// === Control Parameters ===
enum CIRCUITMODE CurrentCircuitMode;
float desiredCurrent = DEFAULT_CURRENT;
float minvoltagedischarg = 3.0;
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
bool pulseState = true;            // true = ON phase, false = OFF phase
unsigned long pulseOnTime = 5000;  // 120000;  // milliseconds ON
unsigned long pulseOffTime = 5000; // 60000;  // milliseconds OFF
unsigned long pulsePreviousMillis = 0;
//******************

void circuitOperationSetup()
{
  pinMode(PWM_PIN_CHARGE, OUTPUT);
  pinMode(SAFETY_RELAY_PIN, OUTPUT);
  pinMode(CHARGE_DISCHARGE_RELAY_PIN, OUTPUT); // relay output
  ledcAttachPin(PWM_PIN_CHARGE, 1);            // Channel 1 for PWM charging
  ledcSetup(1, 20000, 10);                     // 20 kHz, resolution 10-bit

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

void operateCircuit(enum CIRCUITMODE CircuitModeInput)
{
  CurrentCircuitMode = CircuitModeInput;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= Gridtime)
  {

    if (CurrentCircuitMode == CIRCUITOFF)
    {
      handleOff();
      return;
    }
    previousMillis += Gridtime;

    VoltageCurrentReading voltageCurrentS = getVoltageAndCurrent(CurrentCircuitMode);
    measuredVoltage = voltageCurrentS.voltage;
    measuredCurrent = voltageCurrentS.current;
    voltage_diff = voltageCurrentS.voltage_diff;
    // Check pulse state
    bool pulsstate = updatePulseState(currentMillis, CurrentCircuitMode);
    if (pulsstate)
    {
      // Only run this if in ON phase
      if (CurrentCircuitMode == CIRCUITCHARGING)
      {

        handleCharging();
      }
      else if (CurrentCircuitMode == CIRCUITDISCHARGING)
      {
        handleDischarging();
      }
    }
    Serial.println("pulsstate");
    Serial.println(pulsstate);
    digitalWrite(SAFETY_RELAY_PIN, HIGH); // Relay ON during discharging/charging
  }
  // else
  // {
  //   handleOff();
  // }
}

void handleCharging()
{
  ledcWrite(0, 0);
  digitalWrite(CHARGE_DISCHARGE_RELAY_PIN, LOW); // Relay OFF during charging
  voltsShuntAbs = -1 * voltage_diff;             // corrected
  Del_V = voltsChrgMax - measuredVoltage;        // corrected
  error = currDes - 4 * voltsShuntAbs;

  facPwmP_1 = pGainChrg * error;
  iGain_error = iGainChrg * error;

  updateControlAndPWM(measuredVoltage, voltage_diff);
}

void handleDischarging()
{
  ledcWrite(1, 0);
  digitalWrite(CHARGE_DISCHARGE_RELAY_PIN, HIGH); // Relay ON during discharging

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
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  digitalWrite(CHARGE_DISCHARGE_RELAY_PIN, LOW); // Relay ON during discharging
  digitalWrite(SAFETY_RELAY_PIN, LOW);           // Relay ON during discharging
  // Serial.println("Circuit off- cuttting safety relay");
  updateControlAndPWM(measuredVoltage, voltage_diff);
}

void updateControlAndPWM(float measuredVoltage, float voltage_diff)
{
  y = Del_V * 10;
  currLimChrgDisChrg = max(-0.1f, y);
  currDes = min(desiredCurrent, currLimChrgDisChrg);

  facPwmP_2 += (Gridtime / 1000.0) * (iGain_error - derfacPwmAntiWiUp);
  derfacPwmAntiWiUp = T_AntiWiUp * facPwmDeltaLim;
  facPwmDeltaLim = (facPwmP_1 + facPwmP_2) - facPwmLim;
  facPwmLim = constrain(facPwmP_1 + facPwmP_2, 0.0f, 1.0f);

  int pwmValue = constrain((int)(facPwmLim * 1023.0), 0, 1023);
  if (CurrentCircuitMode == CIRCUITCHARGING)
  {
    ledcWrite(1, pwmValue);
  }
  else if (CurrentCircuitMode == CIRCUITDISCHARGING)
  {
    ledcWrite(0, pwmValue);
  }
  else if (CurrentCircuitMode == CIRCUITOFF)
  {
  }

  // // First line: header
  // Serial.println("Voltage | PWM | Measured Current | currDes  | Error | facPwmLim | currLimChrgDisChrg | del_v  | voltage diff ");

  // // Second line: all values on one line
  // Serial.print(measuredVoltage, 3);
  // Serial.print(" | ");
  // Serial.print(pwmValue);
  // Serial.print("     |    ");
  // Serial.print(4 * voltsShuntAbs, 3);
  // Serial.print("   |    ");
  // Serial.print(currDes, 3);
  // Serial.print("    |    ");
  // Serial.print(error, 3);
  // Serial.print("   |   ");
  // Serial.print(facPwmLim, 3);
  // Serial.print(" |    ");
  // Serial.print(currLimChrgDisChrg, 3);
  // Serial.print("   |    ");
  // Serial.print(Del_V, 3);
  // Serial.print(" |    ");
  // Serial.println(voltage_diff, 3);
}

bool updatePulseState(unsigned long currentMillis, enum CIRCUITMODE CurrentCircuitMode)
{
  if (CurrentCircuitMode == CIRCUITCHARGING)
  {
    if (pulseState && (currentMillis - pulsePreviousMillis >= pulseOnTime))
    {
      pulseState = false; // Switch to OFF
      pulsePreviousMillis = currentMillis;
      ledcWrite(1, 0);
    }
    else if (!pulseState && (currentMillis - pulsePreviousMillis >= pulseOffTime))
    {
      pulseState = true; // Switch to ON
      pulsePreviousMillis = currentMillis;
    }
  }
  else if (CurrentCircuitMode == CIRCUITDISCHARGING)
  {
 if (pulseState && (currentMillis - pulsePreviousMillis >= pulseOnTime))
    {
      pulseState = false; // Switch to OFF
      pulsePreviousMillis = currentMillis;
      ledcWrite(0, 1024);
    }
    else if (!pulseState && (currentMillis - pulsePreviousMillis >= pulseOffTime))
    {
      pulseState = true; // Switch to ON
      pulsePreviousMillis = currentMillis;
    }
  }
  return pulseState;
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
  pulseOnTime = verifyValue(pulseOnTimeInput, MIN_PULSE_ON_TIME_S * 1000, MAX_PULSE_ON_TIME_S * 1000, DEFAULT_PULSE_ON_TIME_S * 1000);
}

void setPulseOffTime(unsigned long pulseOffTimeInput)
{
  pulseOffTime = verifyValue(pulseOffTimeInput, MIN_PULSE_OFF_TIME_S * 1000, MAX_PULSE_OFF_TIME_S * 1000, DEFAULT_PULSE_OFF_TIME_S * 1000);
}

void setDesiredCurrent(float desiredCurrentInput)
{
  // desiredCurrent = verifyValue(desiredCurrentInput, MIN_CURRENT, MAX_CURRENT, DEFAULT_CURRENT); //TODO fix
  desiredCurrent = DEFAULT_CURRENT;
}

template <typename T1, typename T2, typename T3, typename T4>
auto verifyValue(T1 value, T2 min, T3 max, T4 defaultValue) -> decltype(value + min + max + defaultValue)
{
  using ReturnType = decltype(value + min + max + defaultValue);
  if (value >= min && value <= max)
  {
    return static_cast<ReturnType>(value);
  }
  else
  {
    Serial.println("Value out of bound, fallingback to default");
    return static_cast<ReturnType>(defaultValue);
  }
}