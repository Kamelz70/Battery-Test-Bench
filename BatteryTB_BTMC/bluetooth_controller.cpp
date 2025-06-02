#include "bluetooth_controller.h"
#include "circuit_config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>  // for notifications
void (*dispatchRecieved)(String recievedString) = NULL;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

class MyServerCallbacks : public BLEServerCallbacks {
  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Client Disconnected - Restarting advertising...");
    // delay(100);
    //    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::getAdvertising()->start();
  }
  void onConnect(BLEServer* pServer) {
    Serial.println("Client Connected");
  }
};
//  BLE recieving callback
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();  // Get incoming data

    if (rxValue.length() > 0) {
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }
      dispatchRecieved(String(rxValue.c_str()));
    }
  }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
void setupBLE(void (*onRecieved)(String recievedString)) {
  BLEDevice::init("ESP32_TestDevice");
  BLEDevice::setMTU(50);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());  // Handles connect/disconnect
  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->addDescriptor(new BLE2902());
  // pCharacteristic->setValue("Hello App Inventor!");
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  dispatchRecieved = onRecieved;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void sendBLEString(String msg) {
  String dataString = "<"+msg+ ">";
  Serial.println("sending"+msg);
  // Serial.print(dataString);
  pCharacteristic->setValue(dataString.c_str());
  pCharacteristic->notify();
  Serial.println("Sent: " + dataString);
}
