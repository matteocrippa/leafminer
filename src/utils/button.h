#ifndef BUTTON_H
#define BUTTON_H
bool button_setup();
#if defined(ESP32)
void buttonTaskFunction(void *pvParameters);
#endif
#endif