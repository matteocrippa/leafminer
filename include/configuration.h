#pragma once

#include <string.h>

#define WIFI_SSID_MAX_LENGTH 32
#define WIFI_PASSWORD_MAX_LENGTH 64
#define WALLET_ADDRESS_MAX_LENGTH 64
#define POOL_PASSWORD_MAX_LENGTH 64
#define POOL_URL_MAX_LENGTH 128
#define BLINK_ENABLED_MAX_LENGTH 5 // "true" or "false"
#define LCD_ON_START_MAX_LENGTH 5 // "true" or "false"
#define MINER_TYPE_MAX_LENGTH 16

typedef struct
{
    char wifi_ssid[WIFI_SSID_MAX_LENGTH + 1];
    char wifi_password[WIFI_PASSWORD_MAX_LENGTH + 1];
    char wallet_address[WALLET_ADDRESS_MAX_LENGTH + 1];
    char pool_password[POOL_PASSWORD_MAX_LENGTH + 1];
    char pool_url[POOL_URL_MAX_LENGTH + 1];
    uint16_t pool_port;
    char blink_enabled[BLINK_ENABLED_MAX_LENGTH + 1];
    uint16_t blink_brightness;
    char lcd_on_start[LCD_ON_START_MAX_LENGTH + 1];
    char miner_type[MINER_TYPE_MAX_LENGTH + 1];
} Configuration;