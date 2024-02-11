#pragma once
#include <stdbool.h>
bool button_setup();
#if defined(ESP32)
void buttonTaskFunction(void *pvParameters);
#endif