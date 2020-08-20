#include <Arduino.h>
#include "utils.h"

void hexString(byte *bytes, size_t bytesLength, char *out)
{
  const char lookup[] = "0123456789ABCDEF";
  for (size_t i = 0; i != bytesLength; ++i)
  {
    out[2 * i] = lookup[bytes[i] >> 4];
    out[2 * i + 1] = lookup[bytes[i] & 0xF];
  }
  out[2 * bytesLength] = '\0'; // null-terminate
}
