#ifndef CIRCUIT_MEASUREMENT_H
#define CIRCUIT_MEASUREMENT_H
#include "custom_types.h"
#include <Arduino.h> 

VoltageCurrentReading getVoltageAndCurrent(enum CIRCUITMODE CircuitMode);
float getTemperature();
String getRealtimeDataString(enum CIRCUITMODE circuitMode);

#endif