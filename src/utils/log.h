#ifndef LOG_H
#define LOG_H
#include <Arduino.h>
void l_info(const char *TAG, PGM_P message, ...);
void l_error(const char *TAG, PGM_P message, ...);
void l_debug(const char *TAG, PGM_P message, ...);
#endif