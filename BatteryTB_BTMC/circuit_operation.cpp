#include "circuit_operation.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "circuit_ads.h"
 Adafruit_ADS1115 ads;

/////////// Helper Functions
void updateControlAndPWM();
void handleCharging();
void handleDischarging();
void handleOff();
void readVoltageAndCurrent();
bool updatePulseState(unsigned long currentMillis);
/////////////////

const int pwmPin = 9;

// === Control Parameters ===
bool flgDisChrg = 1;  // 1 = Discharge, 0 = Charge
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
unsigned long Gridtime = 5;  // ms

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

// for pulse discharging
unsigned long pulseOnTime = 100000;  // milliseconds ON
unsigned long pulseOffTime = 0;  // milliseconds OFF
bool pulseStateOn = true;            // true = ON phase, false = OFF phase
unsigned long pulsePreviousMillis = 0;
//******************

void circuitOperationSetup() {
  pinMode(pwmPin, OUTPUT);

  ledcAttachPin(pwmPin, PWM_CHANNEL);  // Channel 1 for PWM
  ledcSetup(1, 20000, 10);             // 20 kHz, resolution 10-bit

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    // while (1)
    //   ;
  }
  ads.setGain(GAIN_ONE);  // ±4.096V range
}

void operateCircuit(enum CIRCUITMODE CircuitMode) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= Gridtime) {
    previousMillis += Gridtime;

    readVoltageAndCurrent();

    switch (CircuitMode) {
      case CIRCUITCHARGING:
       if (updatePulseState(currentMillis)) {
        handleCharging();}
        break;
      case CIRCUITDISCHARGING:
       if (updatePulseState(currentMillis)) {
        handleDischarging();}
        break;
      case CIRCUITOFF:
        handleOff();
        break;
    }
  }
}

float getCircuitVoltage() {
  return measuredVoltage;
}
float getCircuitCurrent() {
  return measuredCurrent;
}

void handleCharging() {
  voltsShuntAbs = -1 * (measuredCurrent - measuredVoltage);
  x = voltsChrgMax - 2*measuredCurrent;        // Removed "*2" — not using voltage divider ( have a look here)
  error = currDes - (voltsShuntAbs / 0.25);  // Assuming 0.25Ω effective shunt resistance ( have a look here)

  facPwmP_1 = pGainChrg * error;
  iGain_error = iGainChrg * error;

  updateControlAndPWM();
}

void handleDischarging() {
  voltsShuntAbs = measuredCurrent - measuredVoltage;
  x = 2*measuredCurrent - minvoltagedischarg;  // Removed "*2" — not using voltage divider ( have a look here)
  error = currDes - (voltsShuntAbs / 0.25);  // Assuming 0.25Ω effective shunt resistance ( have a look here)

  facPwmP_1 = pGainDisChrg * error;
  iGain_error = iGainDicChrg * error;

  updateControlAndPWM();
}
void handleOff() {
  Serial.println("Unhandled Off");
}

void updateControlAndPWM() {
  y += (1 / T_LowPass) * (Gridtime / 1000.0) * (x - y);
  currLimChrgDisChrg = max(0.0f, (1 / resBat) * y);
  currDes = min(currMaxChrgDisChrg, currLimChrgDisChrg);

  facPwmP_2 += (Gridtime / 1000.0) * (iGain_error - derfacPwmAntiWiUp);
  derfacPwmAntiWiUp = T_AntiWiUp * facPwmDeltaLim;
  facPwmDeltaLim = (facPwmP_1 + facPwmP_2) - facPwmLim;
  facPwmLim = constrain(facPwmP_1 + facPwmP_2, 0.0f, 1.0f);

  int pwmValue = constrain((int)(facPwmLim * 1023.0), 0, 1023.0);
  ledcWrite(PWM_CHANNEL, pwmValue);

  Serial.print("Current: ");
  Serial.print(measuredCurrent, 3);
  Serial.print(" A | Voltage: ");
  Serial.print(measuredVoltage, 3);
  Serial.print(" V | PWM: ");
  Serial.println(pwmValue);
}
bool updatePulseState(unsigned long currentMillis) {
  if (pulseStateOn && (currentMillis - pulsePreviousMillis >= pulseOnTime)) {
    pulseStateOn = false;                      // Switch to OFF
    pulsePreviousMillis = currentMillis;
    ledcWrite(1, 0);                         // Turn off PWM
  }
  else if (!pulseStateOn && (currentMillis - pulsePreviousMillis >= pulseOffTime)) {
    pulseStateOn = true;                       // Switch to ON
    pulsePreviousMillis = currentMillis;
  }

  return pulseStateOn;
}

void readVoltageAndCurrent() {
  // Read battery voltage from A0 (after voltage divider)
  int16_t adc_volt = ads.readADC_SingleEnded(0);
  measuredVoltage = adc_volt * (4.096 / 32767.0);

  // Read differential voltage across shunt (A1 - A3), also through voltage divider
  int16_t adc_diff = ads.readADC_Differential_1_3();
  float voltage_diff = adc_diff * (4.096 / 32768.0);

  // Calculate current using 0.5 ohm shunt
  measuredCurrent = voltage_diff / 0.5;
}

