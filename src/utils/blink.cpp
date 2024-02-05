#include "leafminer.h"
#include "utils/blink.h"
#include "utils/log.h"
#include "model/configuration.h"

#if defined(ESP32)
#define LED_BUILTIN 2
#endif

extern Configuration configuration;

Blink::Blink()
{
}

void Blink::setup()
{
  if (configuration.blink_enabled == "on")
  {
    l_info(TAG_BLINK, "Setup");
    pinMode(LED_BUILTIN, OUTPUT);
    analogWrite(LED_BUILTIN, configuration.blink_brightness);
  }
}

void Blink::blink(int number)
{
  l_info(TAG_BLINK, "Blinking %d times", number);
  if (configuration.blink_enabled == "on")
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