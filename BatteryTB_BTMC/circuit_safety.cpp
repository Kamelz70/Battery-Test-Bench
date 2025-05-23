#include "circuit_safety.h"
#include "circuit_config.h"

CIRCUITSAFETYCODE checkCircuitSafety(float current, float voltage, float temperature)
{

  if (!isTemperatureSafe(temperature))
  {
    return TEMPERATURE_OUT_OF_RANGE;
  }
  if (!isCurrentSafe(current))
  {
    return CURRENT_OUT_OF_RANGE;
  }
  if (!isVoltageSafe(voltage))
  {
    return  VOLTAGE_OUT_OF_RANGE;
  }
  return ALL_SAFE;
}

bool isTemperatureSafe(float temperature)
{
  if (temperature > MAX_TEMPERATURE)
  {
    return false;
  }
  if (temperature < MIN_TEMPERATURE)
  {
    return false;
  }
  return true;
}

bool isCurrentSafe(float current)
{
  if (current > MAX_CURRENT)
  {
    return false;
  }
  if (current < MIN_CURRENT)
  {
    return false;
  }
  return true;
}
bool isVoltageSafe(float voltage)
{
  if (voltage > MAX_VOLTAGE)
  {
    return false;
  }
  if (voltage < MIN_VOLTAGE)
  {
    return false;
  }
  return true;
}