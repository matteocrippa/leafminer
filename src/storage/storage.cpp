#ifdef ESP32
#include <Preferences.h>
#else
#include <EEPROM.h>
#endif // ESP32
#include "storage.h"
#include "utils/log.h"

// void storage_setup()
// {

// }

// void storage_save(const Configuration conf)
// {
//     EEPROM.put(0, conf);
//     EEPROM.commit();
//     EEPROM.end();
// }

// void storage_load(Configuration *conf)
// {
//     try
//     {
//         EEPROM.get(0, *conf);
//         EEPROM.end();
//     }
//     catch (std::exception &e)
//     {
//         l_error("EEPROM", "Error load");
//         l_error("EEPROM", e.what());
//     }
// }

#if defined(ESP32)
Preferences preferences;
#endif

void storage_setup()
{
#if defined(ESP32)
    preferences.begin("config", false);
#endif
}

void storage_save(const Configuration &conf)
{
#if defined(ESP32)
    preferences.putString("wifi_ssid", conf.wifi_ssid.c_str());
    preferences.putString("wifi_password", conf.wifi_password.c_str());
    preferences.putString("wallet_address", conf.wallet_address.c_str());
    preferences.putString("pool_password", conf.pool_password.c_str());
    preferences.putString("pool_url", conf.pool_url.c_str());
    preferences.putInt("pool_port", conf.pool_port);
    preferences.putString("blink_enabled", conf.blink_enabled.c_str());
    preferences.putInt("blink_bright", conf.blink_brightness);
    preferences.putString("lcd_on_start", conf.lcd_on_start.c_str());

    preferences.end();
#endif
}

void storage_load(Configuration *conf)
{
#if defined(ESP32)
    conf->wifi_ssid = preferences.getString("wifi_ssid", "").c_str();
    conf->wifi_password = preferences.getString("wifi_password", "").c_str();
    conf->wallet_address = preferences.getString("wallet_address", "").c_str();
    conf->pool_password = preferences.getString("pool_password", "").c_str();
    conf->pool_url = preferences.getString("pool_url", "public-pool.io").c_str();
    conf->pool_port = preferences.getInt("pool_port", 21496);
    conf->blink_enabled = preferences.getString("blink_enabled", "on").c_str();
    conf->blink_brightness = preferences.getInt("blink_bright", 256);
    conf->lcd_on_start = preferences.getString("lcd_on_start", "on").c_str();
#endif
}

void storage_reset()
{
#if defined(ESP32)
    preferences.clear();
#endif
}