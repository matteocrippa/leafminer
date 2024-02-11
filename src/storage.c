#include "storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define TAG_STORAGE "Storage"

static nvs_handle_t preferences_handle;

esp_err_t storage_setup()
{
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG_STORAGE, "NVS Flash init failed");
        return err;
    }

    err = nvs_open("config", NVS_READWRITE, &preferences_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG_STORAGE, "NVS open failed");
        return err;
    }

    ESP_LOGI(TAG_STORAGE, "Setup: OK");
    return ESP_OK;
}

void storage_save(const Configuration *conf)
{
    nvs_set_str(preferences_handle, "wifi_ssid", conf->wifi_ssid);
    nvs_set_str(preferences_handle, "wifi_password", conf->wifi_password);
    nvs_set_str(preferences_handle, "wallet_address", conf->wallet_address);
    nvs_set_str(preferences_handle, "pool_password", conf->pool_password);
    nvs_set_str(preferences_handle, "pool_url", conf->pool_url);
    nvs_set_u32(preferences_handle, "pool_port", conf->pool_port);
    nvs_set_str(preferences_handle, "blink_enabled", conf->blink_enabled);
    nvs_set_u32(preferences_handle, "blink_brightness", conf->blink_brightness);
    nvs_set_str(preferences_handle, "lcd_on_start", conf->lcd_on_start);
    nvs_commit(preferences_handle);
}

void storage_load(Configuration *conf)
{
    nvs_get_str(preferences_handle, "wifi_ssid", conf->wifi_ssid, sizeof(conf->wifi_ssid));
    nvs_get_str(preferences_handle, "wifi_password", conf->wifi_password, sizeof(conf->wifi_password));
    nvs_get_str(preferences_handle, "wallet_address", conf->wallet_address, sizeof(conf->wallet_address));
    nvs_get_str(preferences_handle, "pool_password", conf->pool_password, sizeof(conf->pool_password));
    nvs_get_str(preferences_handle, "pool_url", conf->pool_url, sizeof(conf->pool_url));
    nvs_get_u16(preferences_handle, "pool_port", &conf->pool_port);
    nvs_get_str(preferences_handle, "blink_enabled", conf->blink_enabled, sizeof(conf->blink_enabled));
    nvs_get_u16(preferences_handle, "blink_brightness", &conf->blink_brightness);
    nvs_get_str(preferences_handle, "lcd_on_start", conf->lcd_on_start, sizeof(conf->lcd_on_start));
}

void storage_close()
{
    nvs_close(preferences_handle);
}
