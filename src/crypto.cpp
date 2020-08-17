#include "Arduino.h"
#include "mbedtls/md.h"
#include "mbedtls/aes.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "crypto.h"

static mbedtls_ctr_drbg_context ctr_drbg;

void cryptoInit()
{
  mbedtls_entropy_context entropy;
  u_char pers[] = "";
  int ret;

  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, pers, sizeof(pers))) != 0)
  {
    Serial.println("Failed to initialize deterministic random bit generator");
  }
}

uint32_t calcENIntervalNumber(uint32_t timestamp)
{
  return timestamp / TIME_WINDOW;
}

void generateTEK(byte *tek)
{
  int ret = mbedtls_ctr_drbg_random(&ctr_drbg, tek, 16);
  if (ret != 0)
  {
    Serial.println("Failed to generate TEK");
  }
}

void generateRPIK(byte *rpik, byte *tek)
{
  const byte rpik_info[] = "EN-RPIK";
  mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, tek, 16, rpik_info, sizeof(rpik_info), rpik, 16);
}

void generateRPI(byte *rpi, byte *rpik, uint32_t enIntervalNumber)
{
  byte padded_data[16] = "EN-RPI";
  padded_data[12] = (enIntervalNumber & 0x000000ff);
  padded_data[13] = (enIntervalNumber & 0x0000ff00) >> 8;
  padded_data[14] = (enIntervalNumber & 0x00ff0000) >> 16;
  padded_data[15] = (enIntervalNumber & 0xff000000) >> 24;

  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, rpik, 16 * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, padded_data, rpi);
  mbedtls_aes_free(&aes);
}

void generateAEMK(byte *aemk, byte *tek)
{
  byte aemkInfo[] = "EN-AEMK";
  mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, tek, 16, aemkInfo, sizeof(aemkInfo), aemk, 16);
}

void generateAEM(byte *aem, byte *aemk, byte *rpi, uint8_t power_level)
{
  byte metadata[4];
  metadata[0] = 0b01000000;
  metadata[1] = power_level;

  size_t count = 0;
  byte sb[16] = {0};
  mbedtls_aes_context aes;

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, aemk, 16 * 8);
  mbedtls_aes_crypt_ctr(&aes, 4, &count, rpi, sb, metadata, aem);
  mbedtls_aes_free(&aes);
}
