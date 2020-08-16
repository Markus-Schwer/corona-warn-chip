/*
 * Titel: Bluetooth Low Energy (BLE) Listener
 * Beschreibung: Ausgabe von RSSI, UUID, Name und Adresse von BLE-Geräten in der Umgebung
 * Letzte Änderung: 15.08.2020
 * Autor: Lukas
 */

#include <BLEAdvertisedDevice.h> // http://www.neilkolban.com/esp32/docs/cpp_utils/html/class_b_l_e_advertised_device.html#a2584fc39d2caa62ea38e2d7bfcf2ccc9
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include <BLEClient.h>

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEClient* pClient;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHAR_UUID              "5c26cd12-4bb8-4d4c-aa2d-ba8975ed23e5"

std::vector<uint8_t> response(200);

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

    ESP_LOGD(LOG_TAG,"******************** Notify callback for characteristic ");
    ESP_LOGD(LOG_TAG,"%s",pBLERemoteCharacteristic->getUUID().toString().c_str());
    ESP_LOGD(LOG_TAG," of data length ");
    ESP_LOGD(LOG_TAG,"%d",length);
    ESP_LOGD(LOG_TAG,"data: ");
    ESP_LOGD(LOG_TAG,"%s",(char*)pData);

    for(int i= 0;i<length;i++) response.push_back(*pData++);

} // end notifyCallback

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Listener erfolgreich gestartet!");
  BLEDevice::init(""); // Initialisierung BLE
}

void loop() {
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true); // Suche in aktiven Modus versetzen für bessere Resultate
  BLEScanResults results = scan->start(5); // Alle 5s suche nach BLE-Devices

  for (int i = 0; i < results.getCount(); i++) { // Schleife für Auslesen aller Bluetooth-Clients
    BLEAdvertisedDevice device = results.getDevice(i); // Bluetooth-Client als 'device' speichern
    
    Serial.print("Gerät Nummer " + i); // Anzahl Geräte
    
    int rssi = device.getRSSI(); // Auslesen der Signalstärke
    Serial.print(F("RSSI -> "));
    Serial.println(rssi); // Ausgabe der Signalstärke
    
    if (device.haveName() == true) { // Abfrage ob Attribut Name vorhanden ist
      Serial.print(F("Device Name -> "));
      Serial.println(device.getName().c_str()); // Ausgabe des Gerätenamens
    }

    if (device.haveServiceUUID() == true) { // Abfrage ob Attribut Service UUID vorhanden ist
      Serial.print(F("Service UUID -> "));
      Serial.println(device.getServiceUUID().toString().c_str()); // Ausgabe der Service UUID
    }

    Serial.print(F("BLE Adresse -> "));
    Serial.println(device.getAddress().toString().c_str()); // Ausgabe der BLE Adresse

    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    pRemoteCharacteristic = pRemoteService->getCharacteristic(CHAR_UUID);

    Serial.printf("CanNotify %d", pRemoteCharacteristic->canNotify());
    Serial.printf("CanRead %d", pRemoteCharacteristic->canRead());
    Serial.printf("CanIndicate %d",pRemoteCharacteristic->canIndicate());
    Serial.printf("CanWrite %d",pRemoteCharacteristic->canWrite());

    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }
    
    if(pRemoteCharacteristic->canNotify()||pRemoteCharacteristic->canIndicate()){
      const uint8_t notifyOn[] = {0x1,0x0};
      //const uint8_t notifyOff[] = {0x0,0x0};
      pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notifyOn,2,true);
      //pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notifyOff,2,true);
      delay(200);
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      delay(400);
    }
    
    Serial.print("");
    Serial.print("");
  }
}
