/*
 * Titel: Bluetooth Low Energy (BLE) Listener
 * Beschreibung: Ausgabe von RSSI, UUID, Name und Adresse von BLE-Geräten in der Umgebung
 * Letzte Änderung: 15.08.2020
 * Autor: Lukas
 */

#include <BLEAdvertisedDevice.h> // http://www.neilkolban.com/esp32/docs/cpp_utils/html/class_b_l_e_advertised_device.html#a2584fc39d2caa62ea38e2d7bfcf2ccc9
#include <BLEDevice.h>
#include <BLEScan.h>

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
    Serial.print("");
    Serial.print("");
  }
}
