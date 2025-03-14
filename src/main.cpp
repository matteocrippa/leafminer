#ifndef UNIT_TEST

#include <Arduino.h>
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
#include <esp_task_wdt.h>

#if defined(HAS_LCD)
#include "screen/screen.h"
#endif // HAS_LCD

char TAG_MAIN[] = "Main";
Configuration configuration;

void setup()
{
  Serial.begin(115200);
  delay(1500);
  l_info(TAG_MAIN, "LeafMiner - v.%s - (C: %d)", _VERSION, CORE);
  l_info(TAG_MAIN, "Compiled: %s %s", __DATE__, __TIME__);
  l_info(TAG_MAIN, "Free memory: %d", ESP.getFreeHeap());

#if defined(ESP32)
  l_info(TAG_MAIN, "Chip Model: %s - Rev: %d", ESP.getChipModel(), ESP.getChipRevision());
  l_info(TAG_MAIN, "Chip ID: %lld", ESP.getEfuseMac());
#else
  l_info(TAG_MAIN, "ESP8266 - Disable WDT");
  ESP.wdtDisable();
  *((volatile uint32_t *)0x60000900) &= ~(1);
#endif // ESP32

  storage_setup();
  bool force_ap = button_setup();

  storage_load(&configuration);
  configuration.print();

  if (configuration.wifi_ssid == "" || force_ap)
  {
#ifdef MASS_WIFI_SSID
    configuration.wifi_ssid = MASS_WIFI_SSID;
    configuration.wifi_password = MASS_WIFI_PASS;
    configuration.pool_url = MASS_POOL_URL;
    configuration.pool_password = MASS_POOL_PASSWORD;
    configuration.pool_port = MASS_POOL_PORT;
    configuration.wallet_address = MASS_WALLET;
#else
    accesspoint_setup();
    return;
#endif // MASS_WIFI_SSID
  }

#if defined(HAS_LCD)
  screen_setup();
#else
  Blink::getInstance().setup();
  delay(500);
  Blink::getInstance().blink(BLINK_START);
#endif // HAS_LCD

  if (configuration.auto_update == "on")
  {
    autoupdate();
  }

  if (network_getJob() == -1)
  {
    l_error(TAG_MAIN, "Failed to connect to network");
    l_info(TAG_MAIN, "Fallback to AP mode");
    accesspoint_setup();
    return;
  }

#if defined(ESP32)
  btStop();
  esp_task_wdt_init(900, true);
  // Idle task that would reset WDT never runs, because core 0 gets fully utilized
  disableCore0WDT();
  xTaskCreatePinnedToCore(currentTaskFunction, "stale", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(networkTaskFunction, "button", 6000, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(mineTaskFunction, "miner0", 6000, (void *)0, 10, NULL, 1);
#if CORE == 2
  xTaskCreatePinnedToCore(mineTaskFunction, "miner1", 6000, (void *)1, 11, NULL, 0);
#endif
#elif defined(ESP8266)
  network_listen();
#endif
}

void loop()
{
  if (configuration.wifi_ssid == "")
  {
    accesspoint_loop();
    return;
  }

#if defined(ESP8266)
  miner(0);
#endif // ESP8266
}

#endif
