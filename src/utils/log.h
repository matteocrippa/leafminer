#ifndef LOG_H
#define LOG_H
#include <Arduino.h>
#include "utils/platform.h"

RAM_ATTR void l_info(const char *TAG, PGM_P message, ...);
RAM_ATTR void l_error(const char *TAG, PGM_P message, ...);
RAM_ATTR void l_debug(const char *TAG, PGM_P message, ...);
#endif