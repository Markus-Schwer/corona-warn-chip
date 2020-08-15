#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCAN_TIME_SECONDS 5
#define FORGET_AFTER_MINUTES 20

BLEScan *scanner;
std::map<std::string, unsigned long> seenNotifiers;

// New exposure notifier is seen -> call
void onNewNotifierFound() {
  Serial.println("DEVICE DETECTED");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called when a BLE device is discovered.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    if (!advertisedDevice.haveServiceUUID()
        || !advertisedDevice.getServiceUUID().equals(BLEUUID((uint16_t) 0xfd6f))
      ) {
      return;
    }

    if (!seenNotifiers.count(advertisedDevice.getAddress().toString())) {
      // New notifier found.
      Serial.printf("+   %s \r\n", advertisedDevice.getAddress().toString().c_str());
      onNewNotifierFound();
    }
    else {
      // We've already seen this one.
      Serial.printf("... %s \r\n", advertisedDevice.getAddress().toString().c_str());
    }

    // Remember, with time.
    seenNotifiers[advertisedDevice.getAddress().toString()] = millis();
  }
};

/**
 * Remove notifiers last seen over FORGET_AFTER_MINUTES ago.
 */
void forgetOldNotifiers() {
  for (auto const &notifier : seenNotifiers) {
    if (notifier.second + (FORGET_AFTER_MINUTES * 60 * 1000) < millis()) {
      Serial.printf("-   %s \r\n", notifier.first.c_str());
      seenNotifiers.erase(notifier.first);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hi.");

   BLEDevice::init("ESP");
   scanner = BLEDevice::getScan();
   scanner->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
   scanner->setActiveScan(true); // Active scan uses more power, but gets results faster.
   scanner->setInterval(100);
   scanner->setWindow(99);

   Serial.println("Scanning ...");
}

void loop() {
  Serial.println("-----");

  // Scan.
  scanner->start(SCAN_TIME_SECONDS, false);

  // Cleanup.
  scanner->clearResults();
  forgetOldNotifiers();

  Serial.printf("Count: %d \r\n", seenNotifiers.size());

}
