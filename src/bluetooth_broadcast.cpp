#include <Arduino.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "utils.h"
#include "crypto.h"
#include "bluetooth_broadcast.h"

static const uint16_t SERVICE_UUID = 0xFD6F;

static esp_ble_adv_params_t advertising_params = {
  .adv_int_min = 0x140, // 200 ms
  .adv_int_max = 0x190, // 250 ms
  .adv_type = ADV_TYPE_NONCONN_IND,
  .own_addr_type = BLE_ADDR_TYPE_RANDOM,
  .peer_addr = {},
  .peer_addr_type = BLE_ADDR_TYPE_RANDOM,
  .channel_map = ADV_CHNL_ALL,
  .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void startAdvertising(void)
{
  ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&advertising_params));
}

void setAdvertisePayload(uint32_t enIntervalNumber, byte *tek)
{
  byte rpik[16];
  byte rpi[16];
  byte aemk[16];
  byte aem[16];

  generateRPIK(rpik, tek);
  generateRPI(rpi, rpik, enIntervalNumber);

  generateAEMK(aemk, tek);
  generateAEM(aem, aemk, rpi, esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV));

  byte advertising_data[31];
  // FLAG
  advertising_data[0] = 0x02;
  advertising_data[1] = 0x01;
  advertising_data[2] = 0x1A;

  // SERVICE UUID
  advertising_data[3] = 0x03;
  advertising_data[4] = 0x03;
  advertising_data[5] = 0x6F;
  advertising_data[6] = 0xFD;

  // SERVICE DATA
  advertising_data[7] = 0x17;
  advertising_data[8] = 0x16;
  advertising_data[9] = 0x6F;
  advertising_data[10] = 0xFD;

  memcpy(&advertising_data[11], rpi, 16);
  memcpy(&advertising_data[11 + 16], aem, 4);

  char dataString[2 * sizeof(advertising_data) + 1];
  hexString(advertising_data, sizeof(advertising_data), dataString);

  Serial.printf("Sending data: %s\n", dataString);

  esp_ble_gap_config_adv_data_raw(advertising_data, sizeof(advertising_data));
}

void stopAdvertising()
{
    ESP_ERROR_CHECK(esp_ble_gap_stop_advertising());
}
