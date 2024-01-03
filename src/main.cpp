#ifndef UNIT_TEST

#include <Arduino.h>

#include "leafminer.h"
#include "utils/log.h"
#include "model/configuration.h"
#include "network/network.h"
#include "utils/blink.h"
#include "miner/miner.h"
#include "current.h"
#include "utils/button.h"
#if defined(ESP32)
#include "freertos/task.h"
#endif // ESP32
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD

char TAG_MAIN[] = "Main";

void setup()
{
  Serial.begin(115200);
  l_debug(TAG_MAIN, "LeafMiner - v.%s", _VERSION);

#if defined(ESP32)
  current_semaphoreInit();
#endif // ESP32
  button_setup();

#if !defined(HAS_LCD)
  Blink::getInstance().setup();
  delay(500);
  Blink::getInstance().blink(BLINK_START);
#endif // HAS_LCD

#if defined(ESP8266)
  l_info(TAG_MAIN, "ESP8266 - Disable WDT");
  ESP.wdtDisable();
  *((volatile uint32_t *)0x60000900) &= ~(1);
#else
  l_info(TAG_MAIN, "ESP32 - Disable WDT");
  disableCore0WDT();
#endif // ESP8266

#if defined(HAS_LCD)
  screen_setup();
#endif // HAS_LCD

#if defined(ESP32)
  xTaskCreatePinnedToCore(mineTaskFunction, "mineTaskCore0", 12192, (void *)0, 1, NULL, 0);
#if CORE == 2
  xTaskCreatePinnedToCore(mineTaskFunction, "mineTaskCore1", 12192, (void *)1, 2, NULL, 1);
#endif // CORE == 2
  xTaskCreatePinnedToCore(buttonTaskFunction, "buttonTask", 2048, NULL, 4, NULL, 0);
#if defined(HAS_LCD)
  xTaskCreatePinnedToCore(screenTaskFunction, "screenTask", 4096, NULL, 3, NULL, 0);
#endif // HAS_LCD
  xTaskCreatePinnedToCore(network_task, "networkTask", 10000, NULL, 5, NULL, 0);
#endif

  network_getJob();
}

void loop()
{
#if defined(ESP8266)
  network_loop();
  miner(0, 0);
#endif // ESP8266
}

#endif