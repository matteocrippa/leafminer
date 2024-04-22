#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <stdio.h>
#include "utils/log.h"

#define TAG_CONFIGURATION "Configuration"

struct Configuration
{
    std::string wifi_ssid = "";
    std::string wifi_password = "";
    std::string wallet_address = "";
    std::string pool_password = "";
    std::string pool_url = "";
    int pool_port = 0;
    std::string blink_enabled = "";
    int blink_brightness = 256;
    std::string lcd_on_start = "";
    std::string miner_type = "";
    std::string auto_update = "";

    void print()
    {
        l_info(TAG_CONFIGURATION, "wifi_ssid: %s", wifi_ssid.c_str());
        l_info(TAG_CONFIGURATION, "wifi_password: %s", wifi_password.c_str());
        l_info(TAG_CONFIGURATION, "wallet_address: %s", wallet_address.c_str());
        l_info(TAG_CONFIGURATION, "pool_password: %s", pool_password.c_str());
        l_info(TAG_CONFIGURATION, "pool_url: %s", pool_url.c_str());
        l_info(TAG_CONFIGURATION, "pool_port: %d", std::to_string(pool_port).c_str());
        l_info(TAG_CONFIGURATION, "blink_enabled: %s", blink_enabled.c_str());
        l_info(TAG_CONFIGURATION, "blink_brightness: %s", std::to_string(blink_brightness).c_str());
        l_info(TAG_CONFIGURATION, "lcd_on_start: %s", lcd_on_start.c_str());
        l_info(TAG_CONFIGURATION, "miner_type: %s", miner_type.c_str());
        l_info(TAG_CONFIGURATION, "auto_update: %s", auto_update.c_str());
    }
};

#endif
