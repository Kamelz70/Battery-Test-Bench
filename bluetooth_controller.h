#ifndef BLUETOOTH_CONTROLLER_H
#define BLUETOOTH_CONTROLLER_H
#include <Arduino.h>

void sendBLEData(char circuitMode);
void setupBLE(void (*onRecieved)(String recievedString));

#endif