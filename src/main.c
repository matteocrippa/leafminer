#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "storage.h" // You need to define storage functions according to your requirements
// #include "utils/button.h" // You need to define button functions according to your requirements
// #include "network/autoupdate.h" // You need to define autoupdate functions according to your requirements
// #include "network/network.h" // You need to define network functions according to your requirements
// #include "miner/miner.h" // You need to define miner functions according to your requirements
// #include "current.h" // You need to define current functions according to your requirements
#include "configuration.h"
#include "leafminer.h"
#include "accesspoint.h"
#include "button.h"

#define TAG_MAIN "Main"

Configuration configuration;
bool force_ap = false;

void setup()
{
    // Initialize storage
    ESP_ERROR_CHECK(storage_setup());

    // Initialize serial communication for debugging
    ESP_LOGI(TAG_MAIN, "LeafMiner - v.%s", _VERSION);

    // Load configuration from storage
    storage_load(&configuration);

    // Setup button
    bool force_ap = button_setup();

    // If no WiFi credentials or force AP mode, set up access point
    if (strcmp(configuration.wifi_ssid, "") == 0 || force_ap)
    {
        // Assuming you have accesspoint_setup function defined
        accesspoint_setup();
        accesspoint_loop();
        // xTaskCreate(&accesspoint_loop, "accessPoint", 4096, NULL, 5, NULL);
        return;
    }

    // Perform autoupdate
    // autoupdate();

    // Connect to network
    // if (network_getJob() == -1)
    // {
    //     ESP_LOGE(TAG_MAIN, "Failed to connect to network");
    //     ESP_LOGI(TAG_MAIN, "Fallback to AP mode");
    //     force_ap = true;
    //     accesspoint_setup();
    //     return;
    // }

    // // Start tasks
    // start_tasks();
}

void loop()
{
    // If in AP mode, handle access point loop
    // if (configuration.wifi_ssid == "" || force_ap)
    // {
    //     accesspoint_loop();
    //     return;
    // }

    // // Run miner task
    // miner(0); // Assuming 0 is the miner index
}

void app_main()
{
    setup();

    // #if defined(ESP32)
    //     xTaskCreate(currentTaskFunction, "stale", 1024, NULL, 1, NULL);
    //     xTaskCreate(buttonTaskFunction, "button", 1024, NULL, 2, NULL);
    //     xTaskCreate(mineTaskFunction, "miner0", 16000, (void *)0, 10, NULL);
    // #if CORE == 2
    //     xTaskCreate(mineTaskFunction, "miner1", 16000, (void *)1, 11, NULL);
    // #endif
    // #endif

    //     network_listen();
}
