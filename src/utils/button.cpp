#include "button.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD
#include <Arduino.h>

#if defined(LILYGO_T_S3)
bool button_setup()
{
  pinMode(14, INPUT_PULLUP);
  if (digitalRead(14) == LOW)
  {
    return true;
  }
  return false;
}
#else
bool button_setup()
{
  return false;
}
#endif // LILYGO_T_S3

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