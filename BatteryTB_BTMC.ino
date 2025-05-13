#include "bluetooth_controller.h"
#include "circuit_operation.h"
#include "custom_types.h"

enum CIRCUITMODE CircuitMode={CIRCUITOFF};
#define LED_PIN 9
/////////////////////////////////////////////////////////callBack function to be sent to setup ble to be called on recieving data
void onRecieved(String recievedString){
    if(recievedString.length()==1)
    {
    switch(recievedString[0])
    {
      case 'c':
      CircuitMode= CIRCUITCHARGING;
      break;
      case 'd':
      CircuitMode= CIRCUITDISCHARGING;
      break;
      case 'o':
      CircuitMode= CIRCUITOFF;
      break;
    }
    
    } }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(LED_PIN, OUTPUT);
  // setup serial
  Serial.begin(115200);
  // BTSerial.begin(9600);     // HC-05 default baud rate
  Serial.println("Serial Started");
  // Turn LED on
  digitalWrite(LED_PIN, LOW);  
  setupBLE(&onRecieved);
  circuitOperationSetup();
}
  unsigned char count=0;

void loop() {
  // put your main code here, to run repeatedly:
    sendBLEData(CircuitMode);
    operateCircuit(CircuitMode);
}
