#ifndef CIRCUIT_CONTROL_H
#define CIRCUIT_CONTROL_H
#include <Arduino.h> 
#include "custom_types.h"

String getBoundarySettingsString();
void parseControlString(String controlString);
CIRCUITMODE getCircuitMode();
void setCircuitMode(enum CIRCUITMODE circuitModeInput);
#endif