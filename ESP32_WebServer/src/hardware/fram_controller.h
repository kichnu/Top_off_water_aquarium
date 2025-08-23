



#ifndef FRAM_CONTROLLER_H
#define FRAM_CONTROLLER_H

#include <Arduino.h>
#include <vector>

// Forward declaration
struct PumpCycle;

// FRAM Memory Map
#define FRAM_ADDR_MAGIC        0x0000  // 4 bytes - magic number for validation
#define FRAM_ADDR_VERSION      0x0004  // 2 bytes - data structure version
#define FRAM_ADDR_VOLUME_ML    0x0006  // 4 bytes - volume per second (float)
#define FRAM_ADDR_CHECKSUM     0x000A  // 2 bytes - simple checksum

// FRAM Memory Map dla cykli (po istniejących danych)
#define FRAM_ADDR_CYCLE_COUNT  0x0020  // 2 bytes - liczba zapisanych cykli
#define FRAM_ADDR_CYCLE_INDEX  0x0022  // 2 bytes - current write index (circular buffer)
#define FRAM_ADDR_CYCLE_DATA   0x0100  // Start danych cykli (256 byte offset)

#define FRAM_MAX_CYCLES        200     // Maksymalnie 200 cykli (~20 dni)
#define FRAM_CYCLE_SIZE        24      // Rozmiar jednego cyklu w bajtach

#define FRAM_MAGIC_NUMBER      0x57415452  // "WATR" in hex
#define FRAM_DATA_VERSION      0x0001

// Basic FRAM functions
bool initFRAM();
bool loadVolumeFromFRAM(float& volume);
bool saveVolumeToFRAM(float volume);
bool verifyFRAM();
void testFRAM();

// Cycle management functions (implemented in fram_controller.cpp)
bool saveCycleToFRAM(const PumpCycle& cycle);
bool loadCyclesFromFRAM(std::vector<PumpCycle>& cycles, uint16_t maxCount = FRAM_MAX_CYCLES);
uint16_t getCycleCountFromFRAM();
bool clearOldCyclesFromFRAM(uint32_t olderThanDays = 14);

#endif