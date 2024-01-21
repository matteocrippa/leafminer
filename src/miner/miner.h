#ifndef MINER_H
#define MINER_H
#include <Arduino.h>
#include "utils/platform.h"
#if defined(ESP32)
void mineTaskFunction(void *pvParameters);
#else
void miner(uint32_t core);
#endif // ESP32
#endif // MINER_H