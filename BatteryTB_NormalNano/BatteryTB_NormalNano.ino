#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "circuit_config.h"
Adafruit_ADS1115 ads;

#define ONE_WIRE_BUS 2 // DS18B20 pin on Arduino Nano

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float voltage_divider_factor = 2;
float measuredVoltage;
float measuredCurrent;
float voltage_diff;
const int safety_relay_Pin = 9; // Pin connected to the relay
unsigned long previousMillis = 0;
unsigned long Gridtime = 5; // ms
float tempC;
void setup()
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(SAFETY_RELAY_PIN, OUTPUT);    // relay output
  digitalWrite(SAFETY_RELAY_PIN, HIGH); // Relay ON during discharging/charging

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1)
      ;
  }
  ads.setGain(GAIN_ONE); // ±4.096V range
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= Gridtime)
  {
    previousMillis += Gridtime;

    readVoltageAndCurrent();
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);

    if (!checkCircuitSafety())
    {
      digitalWrite(SAFETY_RELAY_PIN, LOW); 
    }
    Serial.println(tempC); // Send temperature as plain float text
    delay(200);
  }
}

void readVoltageAndCurrent()
{
  // Read battery voltage from A0 (after voltage divider)
  int16_t adc_volt = ads.readADC_SingleEnded(0);
  measuredVoltage = voltage_divider_factor * adc_volt * (4.096 / 32768.0);

  // Read differential voltage across shunt (A1 - A3), also through voltage divider
  int16_t adc_diff = ads.readADC_Differential_1_3();
  voltage_diff = adc_diff * (4.096 / 32768.0);

  // Calculate current using 0.5 ohm shunt

  measuredCurrent = (abs(voltage_diff) / 0.5) * voltage_divider_factor;
}

bool checkCircuitSafety()
{

  if (tempC > MAX_TEMPERATURE || tempC < MIN_TEMPERATURE)
  {
    return false;
  }
  if (measuredCurrent > MAX_CURRENT || measuredCurrent < MIN_CURRENT)
  {
    return false;
  }
  if (measuredVoltage > MAX_VOLTAGE || measuredVoltage < MIN_VOLTAGE)
  {
    return false;
  }
  return true;
}