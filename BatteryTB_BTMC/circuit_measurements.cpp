#include "circuit_measurements.h"
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;

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