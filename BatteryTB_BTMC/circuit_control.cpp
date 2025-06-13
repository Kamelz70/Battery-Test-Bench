#include "circuit_control.h"
#include "circuit_config.h"

String getBoundarySettingsString()
{
    return String((int)MAX_VOLTAGE, 2) + ","+ //1
    String(MIN_VOLTAGE, 2) + ","+             //2
    String(MAX_CURRENT) + ","+                //3
    String(MIN_CURRENT, 2) + ","+                //4
    String(MAX_TEMPERATURE, 2) + ","+            //5
    String(MIN_TEMPERATURE, 2) + ","+                //6
    String(MAX_STOPTIME) + ","+                //7
    String(MIN_STOPTIME, 2) + ","+                //8
    String(DEFAULT_STOPTIME) + ","+                //9
    String(DEFAULT_VOLTAGE, 2) + ","+                //10
    String(DEFAULT_CURRENT, 2);                //11
}