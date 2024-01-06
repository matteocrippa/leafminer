#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <stdio.h>

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
};

#endif
