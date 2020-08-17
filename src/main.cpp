#include <Arduino.h>
#include <TimeLib.h>
#include "crypto.h"

// enable logging for tinyGSM
// #define GSM_LOGGING

#include "gsm.h"

#ifdef GSM_LOGGING
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
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

void setup()
{
  // Set console baud rate
  SerialMon.begin(9600);

  initializeGsmModem(modem, GSM_PIN);

  while (true)
  {
    if (getGprsConnection(modem, apn, gprsUser, gprsUser))
      break;
    delay(10000);
  }

  time_t unix_epoch_time = 0;
  getGsmTime(modem, &unix_epoch_time);

  setTime(unix_epoch_time);

  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  time_t timestamp = now();
  SerialMon.println(timestamp);

  cryptoInit();
}

void loop()
{
}
