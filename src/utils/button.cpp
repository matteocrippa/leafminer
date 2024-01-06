#include "button.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD
#include <Arduino.h>

bool button_setup()
{
  // TODO: add check for device
  // LILYGO-T-DISPLAY-S3
  pinMode(14, INPUT_PULLUP);

#if defined(ESP32)
// if we start with the button pressed
  if (digitalRead(14) == LOW)
  {
    return true;
  }
  #endif // ESP32
  return false;
}

#if defined(ESP32)
void buttonTaskFunction(void *pvParameters)
{
  while (1)
  {
    // LILYGO-T-DISPLAY-S3
    if (digitalRead(14) == LOW)
    {
#if defined(HAS_LCD)
      screen_toggle();
#endif // HAS_LCD
      delay(333);
    }
    vTaskDelay(333 / portTICK_PERIOD_MS);
  }
}
#endif