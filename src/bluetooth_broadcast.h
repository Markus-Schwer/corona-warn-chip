#ifndef _BLUETOOTH_BROADCAST_H_
#define _BLUETOOTH_BROADCAST_H_

void startAdvertising();

void setAdvertisePayload(uint32_t enIntervalNumber, byte *tek);

void stopAdvertising();

#endif
