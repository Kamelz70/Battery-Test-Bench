#include "circuit_operation.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h> 

/////////// Helper Functions
void updateControlAndPWM();
void handleCharging();
void handleDischarging();
void handleOff();
/////////////////
Adafruit_ADS1115 ads;

const int pwmPin = 9;

// === Control Parameters ===
bool flgDisChrg = 1; // 1 = Discharge, 0 = Charge
float currMaxChrgDisChrg = 2.0;
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
float x = 0;
float derfacPwmAntiWiUp;
float facPwmDeltaLim;
float facPwmLim;
float measuredVoltage;
float measuredCurrent;
void circuitOperationSetup() {
  pinMode(pwmPin, OUTPUT);

  ledcAttachPin(pwmPin, PWM_CHANNEL);              // Channel 1 for PWM
  ledcSetup(1, 20000, 10);               // 20 kHz, resolution 10-bit

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  ads.setGain(GAIN_ONE); // ±4.096V range
}

void operateCircuit(enum CIRCUITMODE CircuitMode) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= Gridtime) {
    previousMillis += Gridtime;

    measuredVoltage = ads.readADC_SingleEnded(ADC_VOLTAGE_CHANNEL) * (4.096 / 65536.0);
    measuredCurrent = ads.readADC_SingleEnded(ADC_CURRENT_CHANNEL) * (4.096 / 65536.0);

   switch(CircuitMode)
    {
      case CIRCUITCHARGING:      
      handleCharging();
      break;
      case CIRCUITDISCHARGING:
      handleDischarging();
      break;
      case CIRCUITOFF:
      handleOff();
      break;
    }

  }
}

float getCircuitVoltage(){
  return measuredVoltage;
}
float getCircuitCurrent(){
  return measuredCurrent;
}

void handleCharging() {
  voltsShuntAbs = -1 * ( measuredCurrent - measuredVoltage);
  x = voltsChrgMax - measuredCurrent;                                    // Removed "*2" — not using voltage divider ( have a look here)
  error = currDes - (voltsShuntAbs / 0.25);                             // Assuming 0.25Ω effective shunt resistance ( have a look here)

  facPwmP_1 = pGainChrg * error;
  iGain_error = iGainChrg * error;

  updateControlAndPWM();
}

void handleDischarging() {
  voltsShuntAbs = measuredCurrent - measuredVoltage;
  x = measuredCurrent - minvoltagedischarg; // Removed "*2" — not using voltage divider ( have a look here)
  error = currDes - (voltsShuntAbs / 0.25); // Assuming 0.25Ω effective shunt resistance ( have a look here)

  facPwmP_1 = pGainDisChrg * error;
  iGain_error = iGainDicChrg * error;

  updateControlAndPWM();
}
void handleOff(){
  
}

void updateControlAndPWM() {
  y += (1 / T_LowPass) * (Gridtime / 1000.0) * (x - y);
  currLimChrgDisChrg = max(0.0f, (1 / resBat) * y);
  currDes = min(currMaxChrgDisChrg, currLimChrgDisChrg);

  facPwmP_2 += (Gridtime / 1000.0) * (iGain_error - derfacPwmAntiWiUp);
  derfacPwmAntiWiUp = T_AntiWiUp * facPwmDeltaLim;
  facPwmDeltaLim = (facPwmP_1 + facPwmP_2) - facPwmLim;
  facPwmLim = constrain(facPwmP_1 + facPwmP_2, 0.0f, 1.0f);

  int pwmValue = constrain((int)(facPwmLim * 255.0), 0, 255);
  ledcWrite(PWM_CHANNEL, pwmValue);

  Serial.print("Current: "); Serial.print(measuredCurrent, 3);
  Serial.print(" A | Voltage: "); Serial.print(measuredVoltage, 3);
  Serial.print(" V | PWM: "); Serial.println(pwmValue);
}
