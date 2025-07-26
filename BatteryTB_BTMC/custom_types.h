#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

enum CIRCUITMODE { CIRCUITCHARGING = 'c',
                   CIRCUITDISCHARGING = 'd',
                   CIRCUITOFF = 'o' };


enum CIRCUITSAFETYCODE { VOLTAGE_OUT_OF_RANGE = 401 ,
                   CURRENT_OUT_OF_RANGE = 402,
                   TEMPERATURE_OUT_OF_RANGE = 403,
                   ALL_SAFE= 201};

struct VoltageCurrentReading {
  float voltage;
  float current;
  float voltage_diff;
  float soc;
};

#endif