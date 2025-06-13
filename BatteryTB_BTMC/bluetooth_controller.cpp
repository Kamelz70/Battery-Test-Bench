#include "bluetooth_controller.h"
#include "circuit_config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>  // for notifications
void (*dispatchRecieved)(String recievedString) = NULL;
void (*dispatchConnect)() = NULL;

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
    dispatchConnect();
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

/* ---------- descriptor callback ---------- */
class CCCDCallbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor* desc) override {
    // cast because BLEDescriptor is the base class
    BLE2902* cccd = (BLE2902*)desc;

    if (cccd->getNotifications()) {  // central wrote 0x01 00
      Serial.println("Central subscribed → sending first packet");
      dispatchConnect();  // or sendBLEString(...)
    }
  }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
void setupBLE(void (*onRecieved)(String recievedString), void (*onConnect)()) {
  BLEDevice::init("ESP32_TestDevice");
  BLEDevice::setMTU(50);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());  // Handles connect/disconnect
  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);
  BLE2902* cccd = new BLE2902();            // create the descriptor
  cccd->setCallbacks(new CCCDCallbacks());  // attach callbacks
  pCharacteristic->addDescriptor(cccd);     // add to characteristic

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  dispatchRecieved = onRecieved;
  dispatchConnect = onConnect;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void sendBLEString(String msg) {
  String dataString = "<" + msg + ">";
  Serial.println("sending: " + msg);
  // Serial.print(dataString);
  pCharacteristic->setValue(dataString.c_str());
  pCharacteristic->notify();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool isBLEConnected() {
  return (pServer && pServer->getConnectedCount() > 0);
}
