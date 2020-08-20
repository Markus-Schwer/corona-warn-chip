#ifndef _GSM_H_
#define _GSM_H_

#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26

// Set serial for debug console (to the Serial Monitor)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#endif

// Define the serial console for debug prints, if needed
#ifdef GSM_LOGGING
#define TINY_GSM_DEBUG SerialMon
#endif

#include <TinyGsmClient.h>

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

void initializeGsmModem(TinyGsm modem, const char *pin, bool restart = false);

bool getGprsConnection(TinyGsm modem, const char *apn, const char *gprsUser, const char *gprsPass);

bool getGsmTime(TinyGsm modem, time_t *unix_epoch_time);

#endif
