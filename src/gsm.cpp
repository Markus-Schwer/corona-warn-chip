#include <Arduino.h>
#include <time.h>

#include "gsm.h"

void initializeGsmModem(TinyGsm modem, const char *pin, bool restart)
{
  delay(10);

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if (restart) modem.restart();
  else modem.init();
  // use modem.init() if you don't need the complete restart

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (modem.getSimStatus() != 3)
  {
    modem.simUnlock(pin);
  }
}

bool getGprsConnection(TinyGsm modem, const char *apn, const char *gprsUser, const char *gprsPass)
{
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");

  if (!modem.isGprsConnected())
  {
    return false;
  }
  SerialMon.println("GPRS connected");
  return true;
}

bool getGsmTime(TinyGsm modem, time_t *unix_epoch_time)
{
  struct tm t;
  SerialMon.println("Getting network time...");
  if (!modem.getGsmLocationTime(&t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec))
  {
    SerialMon.println("fail");
    return false;
  }
  SerialMon.println("success");

  t.tm_year -= 1900;  // tm_year is defined as years since 1900
  t.tm_mon -= 1;      // tm_mon starts at zero
  *unix_epoch_time = mktime(&t);
  return true;
}
