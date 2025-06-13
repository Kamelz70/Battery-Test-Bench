#ifndef CIRCUIT_CONTROL_H
#define CIRCUIT_CONTROL_H
#include <Arduino.h> 

String getBoundarySettingsString();
void parseControlString(String controlString);
String getCircuitMode();

#endif