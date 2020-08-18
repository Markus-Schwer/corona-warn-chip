#include <Arduino.h>
#include "BLE_listener.h"

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

// The remote service we wish to connect to.
static BLEUUID serviceUUID(0xFD6F);
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID(0xFD6F);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

std::vector<uint8_t> response(200);

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str()); // Exposure Notification Service -> muss auf Gerät gespeichert werden!
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void initalizeBLE() {
    SerialMon.println("Starting Arduino BLE Client");
    BLEDevice::init("ESP");

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

void updateCharacteristic() {
    if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
}

/* void scanAndReadBLE() {
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
    } */
}