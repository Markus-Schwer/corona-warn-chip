#include <Arduino.h>
#define CONFIG_BTDM_CONTROLLER_MODE_BLE_ONLY
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "bluetooth.h"

void bleInit()
{
  btStart();

  if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED)
  {
    ESP_ERROR_CHECK(esp_bluedroid_init());
  }
  if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_INITIALIZED)
  {
    ESP_ERROR_CHECK(esp_bluedroid_enable());
  }

  // new bluetooth address nesseccary?
  uint8_t bt_address[ESP_BD_ADDR_LEN];
  esp_fill_random(bt_address, ESP_BD_ADDR_LEN);
  bt_address[0] |= 0xC0;

  ESP_ERROR_CHECK(esp_ble_gap_set_rand_addr(bt_address));

  ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9));
  ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9));
  ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9));
  ESP_ERROR_CHECK(esp_ble_gap_config_local_privacy(true));

  Serial.println("Initialized BLE");
}
