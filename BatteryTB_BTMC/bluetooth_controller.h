#ifndef BLUETOOTH_CONTROLLER_H
#define BLUETOOTH_CONTROLLER_H
#include <Arduino.h>

void sendBLEString(String msg);
void setupBLE(void (*onRecieved)(String recievedString));

#endif