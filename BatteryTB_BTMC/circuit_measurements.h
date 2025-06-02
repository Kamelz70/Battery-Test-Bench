#ifndef CIRCUIT_MEASUREMENT_H
#define CIRCUIT_MEASUREMENT_H
#include "custom_types.h"
#include <Arduino.h> 

VoltageCurrentReading getVoltageAndCurrent();
float getTemperature();
String getRealtimeDataString(String CircuitMode);

#endif