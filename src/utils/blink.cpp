#include "leafminer.h"
#include "utils/blink.h"
#include "utils/log.h"

#if defined(ESP32)
#define LED_BUILTIN 2
#endif

Blink::Blink()
{
}

void Blink::setup()
{
  l_info(TAG_BLINK, "Setup");
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, BLINK_BRIGHTNESS);
}

void Blink::blink(int number)
{
  l_info(TAG_BLINK, "Blinking %d times", number);
  if (BLINK_ENABLED)
  {
    for (int i = 0; i < number; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
    }
  }
}