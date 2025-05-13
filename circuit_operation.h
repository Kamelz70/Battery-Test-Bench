#ifndef CIRCUIT_OPERATION_H
#define CIRCUIT_OPERATION_H
#include <Arduino.h>
#include "custom_types.h"
// Config
#define ADC_VOLTAGE_CHANNEL 0
#define ADC_CURRENT_CHANNEL 1
#define PWM_CHANNEL 1

void circuitOperationSetup();
void operateCircuit(enum CIRCUITMODE CircuitMode);
float getCircuitVoltage();
float getCircuitCurrent();

#endif