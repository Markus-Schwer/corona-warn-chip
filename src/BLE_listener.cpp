#include <Arduino.h>
#include "BLE_listener.h"

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEClient* pClient;

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

void initalizeBLE() {
    BLEDevice::init("ESP");
}

void scanAndReadBLE() {
    BLEScan *scan = BLEDevice::getScan();
    scan->setActiveScan(true);
    BLEScanResults results = scan->start(5);

    for (int i = 0; i < results.getCount(); i++) {
        BLEAdvertisedDevice device = results.getDevice(i);

        BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
        pRemoteCharacteristic = pRemoteService->getCharacteristic(CHAR_UUID);

        Serial.printf("CanNotify %d", pRemoteCharacteristic->canNotify());
        Serial.printf("CanRead %d", pRemoteCharacteristic->canRead());
        Serial.printf("CanIndicate %d",pRemoteCharacteristic->canIndicate());
        Serial.printf("CanWrite %d",pRemoteCharacteristic->canWrite());

        if (pRemoteCharacteristic->canRead()) {
            std::string value = pRemoteCharacteristic->readValue();
            SerialMon.print("The characteristic value was: ");
            SerialMon.println(value.c_str());
        }

        if (pRemoteCharacteristic->canNotify() || pRemoteCharacteristic->canIndicate()) {
            const uint8_t notifyOn[] = {0x1,0x0};
            pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notifyOn,2,true);
            delay(200);
            pRemoteCharacteristic->registerForNotify(notifyCallback);
            delay(400);
        }

        SerialMon.print("");
        SerialMon.print("");
    }
}