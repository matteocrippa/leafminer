#include "button.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD
#include <Arduino.h>

#define BUTTON_PIN -1
#if defined(LILYGO_T_S3)
#define BUTTON_PIN 14
#endif
#if defined(ESP32_S3) || defined(ESP32_WROOM)
#define BUTTON_PIN 0
#endif

bool button_setup()
{
  if (BUTTON_PIN != -1)
  {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      return true;
    }
  }
  return false;
}

#if defined(ESP32)

#if defined(LILYGO_T_S3)
void button_task()
{
  if (digitalRead(14) == LOW)
  {
#if defined(HAS_LCD)
    screen_toggle();
#endif // HAS_LCD
    delay(333);
  }
}
#else
void button_task()
{
}
#endif // LILYGO_T_S3

void buttonTaskFunction(void *pvParameters)
{
  while (1)
  {
    button_task();
    vTaskDelay(333 / portTICK_PERIOD_MS);
  }
}
#endif