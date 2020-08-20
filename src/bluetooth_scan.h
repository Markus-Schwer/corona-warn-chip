#ifndef _BLUETOOTH_SCAN_H_
#define _BLUETOOTH_SCAN_H_

typedef enum
{
  SCAN_STATUS_SCANNING = 0, // scan is running
  SCAN_STATUS_NOT_SCANNING, // scan is not running
  SCAN_STATUS_WAITING,      // scan was stopped manually
} ble_scan_status;

void scanInit();

void startScanning(uint32_t duration);

ble_scan_status getScanStatus();

#endif
