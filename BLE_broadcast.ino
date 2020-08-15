/*
 * Titel: Bluetooth Low Energy Broadcast
 * Beschreibung:
 * Letzte Änderung: 15.08.2020
 * Autor: Lukas
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SERVICE_UUID        "e5d49329-fd4e-4506-89a3-79f2b29ce108"
#define CHARACTERISTIC_UUID "28ea530d-2663-44fd-a58b-9cde851efebe"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Server gestartet!");

  // BLE initialisieren
  BLEDevice::init("BLE broadcast"); 

  // BLE Server erstellen
  BLEServer *pServer = BLEDevice::createServer(); 
  pServer->setCallbacks(new MyServerCallbacks());

  // BLE Server Service UUID zuweisen
  BLEService *pService = pServer->createService(SERVICE_UUID); 

  // BLE Charakteristiken erstellen
  BLECharacteristic *pCharacteristic = pService->createCharacteristic( 
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ   |
                                         BLECharacteristic::PROPERTY_WRITE  |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  
  pCharacteristic->addDescriptor(new BLE2902());
  //pCharacteristic->setValue("Hello here ist BLE broadcast");

  // Service starten
  pService->start(); 

  // Advertising starten
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); 
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  //pAdvertising->setMinPreferred(0x06);
  //pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();
  Serial.println("Auf Client-Verbindung warten ...");
}

void loop() {
   // Wert-Änderung benachrichtigen
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->notify();
        value++;
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

}
