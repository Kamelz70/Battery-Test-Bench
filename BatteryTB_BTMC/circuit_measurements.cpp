#include "circuit_measurements.h"
#include <Adafruit_ADS1X15.h>
#include "circuit_config.h"
#include "circuit_ads.h"

///////////////////// Mock data ranges
// #define MIN_VOLTAGE 3.0
// #define MAX_VOLTAGE 4.2
// #define MIN_CURRENT -5.0  // Negative for discharging
// #define MAX_CURRENT 5.0   // Positive for charging
#define MIN_TEMP 20.0
#define MAX_TEMP 45.0
#define MIN_SOH 80.0
#define MAX_SOH 100.0
#define MIN_SOC 0.0
#define MAX_SOC 100.0

VoltageCurrentReading getVoltageAndCurrent()
{
  VoltageCurrentReading result;
  // Read battery voltage from A0 (after voltage divider)
  int16_t adc_volt = ads.readADC_SingleEnded(0);
  result.voltage = adc_volt * (4.096 / 32767.0);

  // Read differential voltage across shunt (A1 - A3), also through voltage divider
  int16_t adc_diff = ads.readADC_Differential_1_3();
  float voltage_diff = adc_diff * (4.096 / 32768.0);

  // Calculate current using 0.5 ohm shunt
  result.current = voltage_diff / 0.5;

  return result;
}

float getTemperature() {
  return 100.1;
}
String getRealtimeDataString(String circuitMode)
{
    // Generate random mock data within ranges
  float voltage = MIN_VOLTAGE + (rand() % (int)((MAX_VOLTAGE - MIN_VOLTAGE) * 100)) / 100.0;
  float current = MIN_CURRENT + (rand() % (int)((MAX_CURRENT - MIN_CURRENT) * 100)) / 100.0;
  float temperature = MIN_TEMP + (rand() % (int)((MAX_TEMP - MIN_TEMP) * 100)) / 100.0;
  float soh = MIN_SOH + (rand() % (int)((MAX_SOH - MIN_SOH) * 100)) / 100.0;
  float soc = MIN_SOC + (rand() % (int)((MAX_SOC - MIN_SOC) * 100)) / 100.0;

  // VoltageCurrentReading voltageCurrent=getVoltageAndCurrent();

  // return String(voltageCurrent.voltage, 2) + ","
  // + String(voltageCurrent.current, 2) + ","
  
  return String(voltage, 2) + ","
  + String(current, 2) + ","
  + String(temperature, 2) + ","
  + String(soh, 2) + ","
  + String(soc, 2) + ","
  + circuitMode;
}