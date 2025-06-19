#ifndef CIRCUIT_MEASUREMENT_H
#define CIRCUIT_MEASUREMENT_H
#include "custom_types.h"
#include <Arduino.h> 

VoltageCurrentReading getVoltageAndCurrent(enum CIRCUITMODE CircuitMode);
String getRealtimeDataString(enum CIRCUITMODE circuitMode);
void measurementSetup();
float getSavedTemperature();
void updateTemperature();
#endif