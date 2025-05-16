#include "bluetooth_controller.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>  // for notifications
void (*dispatchRecieved)(String recievedString) = NULL;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

//////////////////////// Your custom BLE UUIDs
#define SERVICE_UUID "58f4c711-037d-4dd5-8b2c-80a46b3d9ab4"
#define CHARACTERISTIC_UUID "31c9dd7b-e55d-4e99-8b8a-e7a00857949a"


///////////////////// Mock data ranges
#define MIN_VOLTAGE 3.0
#define MAX_VOLTAGE 4.2
#define MIN_CURRENT -5.0  // Negative for discharging
#define MAX_CURRENT 5.0   // Positive for charging
#define MIN_TEMP 20.0
#define MAX_TEMP 45.0
#define MIN_SOH 80.0
#define MAX_SOH 100.0
#define MIN_SOC 0.0
#define MAX_SOC 100.0


class MyServerCallbacks : public BLEServerCallbacks {
  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Client Disconnected - Restarting advertising...");
    // delay(100);
    BLEDevice::getAdvertising()->start();
  }
  void onConnect(BLEServer* pServer) {
    Serial.println("Client Connected");
  }

  //    void onDisconnect(BLEServer* pServer) {
  //   Serial.println("Client Disconnected - Restarting advertising...");
  //     BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  //    pAdvertising->addServiceUUID(SERVICE_UUID);
  //    pAdvertising->start();
  // }
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
void sendBLEData(char circuitMode) {


  // Generate random mock data within ranges
  float voltage = MIN_VOLTAGE + (rand() % (int)((MAX_VOLTAGE - MIN_VOLTAGE) * 100)) / 100.0;
  float current = MIN_CURRENT + (rand() % (int)((MAX_CURRENT - MIN_CURRENT) * 100)) / 100.0;
  float temperature = MIN_TEMP + (rand() % (int)((MAX_TEMP - MIN_TEMP) * 100)) / 100.0;
  float soh = MIN_SOH + (rand() % (int)((MAX_SOH - MIN_SOH) * 100)) / 100.0;
  float soc = MIN_SOC + (rand() % (int)((MAX_SOC - MIN_SOC) * 100)) / 100.0;

  // Format the data string (CSV format)
  String dataString = "<"
                      + String(voltage, 2) + ","
                      + String(current, 2) + ","
                      + String(temperature, 2) + ","
                      + String(soh, 2) + ","
                      + String(soc, 2) + ","
                      + circuitMode
                      + ">";
  // Send via Bluetooth (assuming your bluetooth_controller has this function)
  // Serial.print(dataString);
  pCharacteristic->setValue(dataString.c_str());
  pCharacteristic->notify();
  Serial.println("Sent: " + dataString);
}
