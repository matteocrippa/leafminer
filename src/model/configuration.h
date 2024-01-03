#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <stdio.h>
#include "utils/platform.h"

struct Configuration
{
    std::string wifi_ssid;
    std::string wifi_password;
    std::string address;
    std::string password;
    std::string pool_url;
    int pool_port;

    // Constructor using member initialization list
    Configuration(const std::string &wifi_ssid, const std::string &wifi_password, const std::string &address, const std::string &password, const std::string &pool_url, const int &pool_port)
        : wifi_ssid(wifi_ssid),
          wifi_password(wifi_password),
          address(address),
          password(password),
          pool_url(pool_url),
          pool_port(pool_port)
    {
    }
};

#endif
