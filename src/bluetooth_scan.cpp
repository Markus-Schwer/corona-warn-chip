#include <Arduino.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "utils.h"
#include "bluetooth_scan.h"

static ble_scan_status scan_status = SCAN_STATUS_NOT_SCANNING;

const uint16_t SERVICE_UUID = 0xFD6F;

static esp_ble_scan_params_t scan_params = {
    .scan_type = BLE_SCAN_TYPE_PASSIVE,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval = 0x30, // don't know good parameters, just copied
    .scan_window = 0x2f,   // don't know good parameters, just copied
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
};

void scanEventCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  switch (event)
  {
  case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
    Serial.println("Started scanning...");
    break;
  case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
    Serial.println("Stopped scanning");
    break;
  case ESP_GAP_BLE_SCAN_RESULT_EVT:
    if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT)
    {
      uint8_t service_uuid_length;
      byte *service_uuid = esp_ble_resolve_adv_data(param->scan_rst.ble_adv, 0x03, &service_uuid_length);

      if (service_uuid_length == sizeof(SERVICE_UUID) && memcmp(service_uuid, &SERVICE_UUID, service_uuid_length) == 0)
      {
        uint8_t service_data_length = 0;
        byte *service_data = esp_ble_resolve_adv_data(param->scan_rst.ble_adv, 0x16, &service_data_length);

        // length | data
        //    2   | service uuid (0x6FFD)
        //   16   | Rolling Proximity Identifier
        //    4   | Associated Encrypted Metadata

        if (service_data_length != 22)
        {
          Serial.println("Received invalid payload");
          break;
        }

        byte rpi[16];
        memcpy(rpi, &service_data[2], 16);
        byte aem[4];
        memcpy(aem, &service_data[2 + 16], 4);

        time_t timestamp = time(NULL);

        char rpiString[33];
        hexString(rpi, 16, rpiString);
        char aemString[9];
        hexString(aem, 4, aemString);

        Serial.printf("Recieved payload: timestamp: %d, rpi: %s, aem: %s\n", timestamp, rpiString, aemString);
      }
    }
    break;
  default:
    break;
  }
}

void scanInit()
{
  ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&scan_params));
  ESP_ERROR_CHECK(esp_ble_gap_register_callback(scanEventCallback));
}

void startScanning(uint32_t duration)
{
  scan_status = SCAN_STATUS_SCANNING;
  ESP_ERROR_CHECK(esp_ble_gap_start_scanning(duration));
}

void stopScanning()
{
  scan_status = SCAN_STATUS_WAITING;
  ESP_ERROR_CHECK(esp_ble_gap_stop_scanning());
}

ble_scan_status getScanStatus()
{
  return scan_status;
}
