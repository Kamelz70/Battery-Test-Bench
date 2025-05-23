#ifndef CIRCUIT_SAFETY_H
#define CIRCUIT_SAFETY_H
#include "custom_types.h"

CIRCUITSAFETYCODE checkCircuitSafety(float current, float voltage, float temperature);
bool isTemperatureSafe(float temperature);
bool isCurrentSafe(float current);
bool isVoltageSafe(float voltage);

#endif