#include "circuit_control.h"
#include "circuit_config.h"

String getBoundarySettingsString()
{
    return String((int)MAX_VOLTAGE, 2) + ","+
    String(MIN_VOLTAGE, 2) + ","+
    String(MAX_CURRENT) + ","+
    String(MIN_CURRENT, 2) + ","+
    String(MAX_TEMPERATURE, 2) + ","+
    String(MIN_TEMPERATURE, 2) + ","+
    String(MAX_STOPTIME) + ","+
    String(MIN_STOPTIME, 2) + ","+
    String(DEFAULT_VOLTAGE, 2) + ","+
    String(DEFAULT_CURRENT, 2);
}