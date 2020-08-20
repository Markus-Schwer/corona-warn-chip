#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#define TIME_WINDOW 600        // time window is 10 minutes
#define TEK_ROLLING_PERIOD 144 // TEKRollingPeriod

void cryptoInit();

uint32_t calcENIntervalNumber(uint32_t unix_timestamp);

void generateTEK(byte *tek);

void generateRPIK(byte *rpik, byte *tek);

void generateRPI(byte *rpi, byte *rpik, uint32_t enIntervalNumber);

void generateAEMK(byte *aemk, byte *tek);

void generateAEM(byte *aem, byte *aemk, byte *rpi, uint8_t power_level);

#endif
