#ifndef CIRCUIT_MEASUREMENT_H
#define CIRCUIT_MEASUREMENT_H
#include "custom_types.h"
#include <Arduino.h> 

VoltageCurrentReading getVoltageAndCurrent(enum CIRCUITMODE CircuitMode);
float getSOC(enum CIRCUITMODE CircuitMode);
String getRealtimeDataString(enum CIRCUITMODE circuitMode,bool pulseState);
void measurementSetup();
float getSavedTemperature();
void updateTemperature();
#endif