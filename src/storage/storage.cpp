#include <Preferences.h>
#include "storage.h"
#include "utils/log.h"

Preferences preferences;

const char TAG_STORAGE[13] = "Storage";

void storage_setup()
{
    bool success = preferences.begin("config", false);
    l_info(TAG_STORAGE, "Setup: %s", success ? "OK" : "ERROR");
}

void storage_save(const Configuration &conf)
{
    preferences.putString("wifi_ssid", conf.wifi_ssid.c_str());
    preferences.putString("wifi_password", conf.wifi_password.c_str());
    preferences.putString("wallet_address", conf.wallet_address.c_str());
    preferences.putString("pool_password", conf.pool_password.c_str());
    preferences.putString("pool_url", conf.pool_url.c_str());
    preferences.putUInt("pool_port", conf.pool_port);
    preferences.putString("blink_enabled", conf.blink_enabled.c_str());
    preferences.putUInt("blink_bright", conf.blink_brightness);
    preferences.putString("lcd_on_start", conf.lcd_on_start.c_str());
    preferences.putString("auto_update", conf.auto_update.c_str());
    preferences.end();
}

void storage_load(Configuration *conf)
{
    conf->wifi_ssid = preferences.getString("wifi_ssid", "").c_str();
    conf->wifi_password = preferences.getString("wifi_password", "").c_str();
    conf->wallet_address = preferences.getString("wallet_address", "").c_str();
    conf->pool_password = preferences.getString("pool_password", "").c_str();
    conf->pool_url = preferences.getString("pool_url", "pool.vkbit.com").c_str();
    conf->pool_port = preferences.getUInt("pool_port", 3333);
    conf->blink_enabled = preferences.getString("blink_enabled", "on").c_str();
    conf->blink_brightness = preferences.getUInt("blink_bright", 256);
    conf->lcd_on_start = preferences.getString("lcd_on_start", "on").c_str();
    conf->auto_update = preferences.getString("auto_update", "on").c_str();
}
