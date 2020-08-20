#include <Arduino.h>
#include <sys/time.h>
#include "crypto.h"
#include "bluetooth.h"
#include "bluetooth_scan.h"
#include "bluetooth_broadcast.h"

// enable logging for tinyGSM
// #define GSM_LOGGING

#include "gsm.h"

#ifdef GSM_LOGGING
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

TinyGsmClient client(modem);

static byte tek[16];

void setup()
{
  // Set console baud rate
  Serial.begin(9600);

  initializeGsmModem(modem, GSM_PIN);

  while (true)
  {
    if (getGprsConnection(modem, apn, gprsUser, gprsUser))
      break;
    delay(10000);
  }

  time_t unix_epoch_time = 0;
  while (true)
  {
    if (getGsmTime(modem, &unix_epoch_time))
      break;
    initializeGsmModem(modem, GSM_PIN, true);

    while (true)
    {
      if (getGprsConnection(modem, apn, gprsUser, gprsUser))
        break;
      delay(10000);
    }

    delay(10000);
  }

  struct timeval gsmTime = {.tv_sec = unix_epoch_time};
  settimeofday(&gsmTime, NULL);

  modem.gprsDisconnect();
  Serial.println(F("GPRS disconnected"));

  cryptoInit();

  bleInit();
  scanInit();
  startAdvertising();

  byte tek[16];
  generateTEK(tek);
}

void loop()
{
  time_t timestamp = time(NULL);
  uint32_t enIntervalNumber = calcENIntervalNumber(timestamp);
  setAdvertisePayload(enIntervalNumber, tek);
  startScanning(5);
  delay(10000);
}
