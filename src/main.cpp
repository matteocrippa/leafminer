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
#include "massdeploy.h"
#if defined(ESP32)
#include <WiFi.h>
#include "freertos/task.h"
#endif // ESP32
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
#if defined(MASS_WIFI_SSID) && defined(MASS_WIFI_PASS) && defined(MASS_POOL_URL) && defined(MASS_POOL_PASSWORD) && defined(MASS_POOL_PORT) && defined(MASS_WALLET)
    configuration.wifi_ssid = MASS_WIFI_SSID;
    configuration.wifi_password = MASS_WIFI_PASS;
    configuration.pool_url = MASS_POOL_URL;
    configuration.pool_password = MASS_POOL_PASSWORD;
    configuration.pool_port = MASS_POOL_PORT;
    configuration.wallet_address = MASS_WALLET;
#else
    accesspoint_setup();
    return;
#endif // MASS_WIFI_SSID && MASS_WIFI_PASS && MASS_SERVER_DOMAIN && MASS_SERVER_PASSWORD && MASS_WALLET
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
  xTaskCreate(mineTaskFunction, "miner0", 16000, (void *)0, 10, NULL);
#if CORE == 2
  l_info(TAG_MAIN, "ESP32 - Dual core");
  xTaskCreate(mineTaskFunction, "miner1", 16000, (void *)1, 11, NULL);
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