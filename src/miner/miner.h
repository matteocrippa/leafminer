#ifndef MINER_H
#define MINER_H
#include <Arduino.h>
#include "utils/platform.h"
#if defined(ESP32)
void mineTaskFunction(void *pvParameters);
#else
RAM_ATTR void miner(uint32_t core, uint32_t from);
#endif // ESP32
#endif // MINER_H