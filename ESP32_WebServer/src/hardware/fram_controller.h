#ifndef FRAM_CONTROLLER_H
#define FRAM_CONTROLLER_H

#include <Arduino.h>

// FRAM Memory Map
#define FRAM_ADDR_MAGIC        0x0000  // 4 bytes - magic number for validation
#define FRAM_ADDR_VERSION      0x0004  // 2 bytes - data structure version
#define FRAM_ADDR_VOLUME_ML    0x0006  // 4 bytes - volume per second (float)
#define FRAM_ADDR_CHECKSUM     0x000A  // 2 bytes - simple checksum

#define FRAM_MAGIC_NUMBER      0x57415452  // "WATR" in hex
#define FRAM_DATA_VERSION      0x0001

bool initFRAM();
bool loadVolumeFromFRAM(float& volume);
bool saveVolumeToFRAM(float volume);
bool verifyFRAM();
void testFRAM();

#endif