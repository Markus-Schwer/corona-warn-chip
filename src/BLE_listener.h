#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include <BLEClient.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHAR_UUID              "5c26cd12-4bb8-4d4c-aa2d-ba8975ed23e5"

#define SerialMon Serial

void initalizeBLE();

void scanAndReadBLE();