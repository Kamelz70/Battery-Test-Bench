#ifndef CIRCUIT_SAFETY_H
#define CIRCUIT_SAFETY_H
#include "custom_types.h"
#include <Arduino.h> 
CIRCUITSAFETYCODE checkCircuitSafety(float current, float voltage, float temperature,enum CIRCUITMODE circuitMode);
bool isTemperatureSafe(float temperature);
bool isCurrentSafe(float current,enum CIRCUITMODE circuitMode);
bool isVoltageSafe(float voltage,enum CIRCUITMODE circuitMode);
String getSafetyCodeString(CIRCUITSAFETYCODE safetyCode);

#endif