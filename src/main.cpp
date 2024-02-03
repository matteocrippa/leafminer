#ifndef UNIT_TEST

#include <Arduino.h>

#if defined(ESP32)
#include "freertos/task.h"
#endif // ESP32

#include "leafminer.h"
#include "utils/log.h"
#include "model/configuration.h"
#include "network/network.h"
#include "network/accesspoint.h"
#include "utils/blink.h"
#include "miner/miner.h"
#include "current.h"
#include "utils/button.h"
#include "storage/storage.h"
#include "network/autoupdate.h"
<<<<<<< HEAD
#if defined(ESP32)
#include <WiFi.h>
#include "freertos/task.h"
#endif // ESP32
    =======
>>>>>>> cbae08a0b490bea3dd3ce12e306c6464355b3d63
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD

    char TAG_MAIN[] = "Main";
Configuration configuration = Configuration();
bool force_ap = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  l_debug(TAG_MAIN, "LeafMiner - v.%s", _VERSION);

#if defined(ESP8266)
  l_info(TAG_MAIN, "ESP8266 - Disable WDT");
  ESP.wdtDisable();
  *((volatile uint32_t *)0x60000900) &= ~(1);
#else
  l_info(TAG_MAIN, "ESP32 - Disable WDT");
  disableCore0WDT();
#endif // ESP8266

  storage_setup();

  force_ap = button_setup();

  storage_load(&configuration);
  if (configuration.wifi_ssid == "" || force_ap)
  {
    accesspoint_setup();
    return;
  }

#if !defined(HAS_LCD)
  Blink::getInstance().setup();
  delay(500);
  Blink::getInstance().blink(BLINK_START);
#else
  screen_setup();
#endif // HAS_LCD

#if defined(ESP32)
  btStop();
  l_info(TAG_MAIN, "ESP32 - Stale task");
  xTaskCreate(currentTaskFunction, "stale", 1024, NULL, 1, NULL);
  l_info(TAG_MAIN, "ESP32 - Button task");
  xTaskCreate(buttonTaskFunction, "button", 1024, NULL, 2, NULL);
#if defined(HAS_LCD)
  l_info(TAG_MAIN, "ESP32 - Screen task");
  xTaskCreate(screenTaskFunction, "screen", 1024, NULL, 3, NULL);
#endif
  xTaskCreate(mineTaskFunction, "miner0", 14000, (void *)0, 21, NULL);
#if CORE == 2
  l_info(TAG_MAIN, "ESP32 - Dual core");
  xTaskCreate(mineTaskFunction, "miner1", 14000, (void *)1, 20, NULL);
#endif
#endif

  autoupdate();

  if (network_getJob() == -1)
  {
    l_error(TAG_MAIN, "Failed to connect to network");
    l_info(TAG_MAIN, "Fallback to AP mode");
    force_ap = true;
    accesspoint_setup();
    return;
  }
  network_listen();
}

void loop()
{
  if (configuration.wifi_ssid == "" || force_ap)
  {
    accesspoint_loop();
    return;
  }

#if defined(ESP8266)
  miner(0);
#endif // ESP8266
}

#endif