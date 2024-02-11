#include "accesspoint.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "html_setup.h"
#include "html_ota.h"
#include "storage.h"
#include "model/configuration.h"

#define TAG_AP "AccessPoint"
#define AP_SSID "LEAFMINER"
#define AP_DNS_PORT 53

httpd_handle_t server = NULL;

extern Configuration configuration;

esp_err_t accesspoint_upload_handler(httpd_req_t *req) {
    httpd_resp_send(req, "Uploading firmware...<br/><br/>Please wait...<br/><br/>Device will reboot automatically after update is completed.<br/><br/>", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void accesspoint_webserver(const Configuration *configuration) {
    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            char *html = prepareHtmlWithValues(configuration);
            httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
            free(html);
        }
    };

    httpd_uri_t ota_uri = {
        .uri = "/ota",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            httpd_resp_send(req, html_ota, HTTPD_RESP_USE_STRLEN);
        }
    };

    httpd_uri_t upload_uri = {
        .uri = "/upload",
        .method = HTTP_POST,
        .handler = accesspoint_upload_handler
    };

    httpd_uri_t save_uri = {
        .uri = "/save",
        .method = HTTP_POST,
        .handler = [](httpd_req_t *req) {
            char buf[1024];
            size_t recv_size = httpd_req_recv(req, buf, sizeof(buf));
            if (recv_size > 0) {
                buf[recv_size] = '\0';
                Configuration conf;
                sscanf(buf, "%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%d&%*[^=]=%[^&]&%*[^=]=%d&%*[^=]=%[^&]", 
                        conf.wifi_ssid, conf.wifi_password, conf.wallet_address, conf.pool_password, conf.pool_url, &conf.pool_port, conf.blink_enabled, 
                        &conf.blink_brightness, conf.lcd_on_start);
                storage_save(&conf);
                httpd_resp_send(req, "Data saved successfully!<br/><br/>Please reboot your board!", HTTPD_RESP_USE_STRLEN);
            } else {
                httpd_resp_send(req, "Failed to save data!", HTTPD_RESP_USE_STRLEN);
            }
        }
    };

    httpd_uri_t notfound_uri = {
        .uri = "/*",
        .method = HTTP_ANY,
        .handler = [](httpd_req_t *req) {
            httpd_resp_send(req, "", 0);
            return ESP_OK;
        }
    };

    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &ota_uri);
    httpd_register_uri_handler(server, &upload_uri);
    httpd_register_uri_handler(server, &save_uri);
    httpd_register_uri_handler(server, &notfound_uri);
}

void accesspoint_setup() {
    ESP_LOGI(TAG_AP, "Setup");

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .max_connection = 4,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("leafminer"));
    ESP_ERROR_CHECK(mdns_instance_name_set("LeafMiner Access Point"));

    ESP_ERROR_CHECK(nvs_flash_init());

    tcpip_adapter_dns_info_t dns_info = {
        .ip = { 192, 168, 4, 1 }
    };
    ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_AP, TCPIP_ADAPTER_DNS_MAIN, &dns_info));

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    accesspoint_webserver(&configuration);
}

void accesspoint_loop() {
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

/*
#include "accesspoint.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "storage.h"
#include "model/configuration.h"

#define TAG_AP "AccessPoint"
#define AP_SSID "LEAFMINER"
#define AP_DNS_PORT 53

httpd_handle_t server = NULL;

extern Configuration configuration;

/* URI handler function for GET / */
esp_err_t root_get_handler(httpd_req_t *req) {
    char *html = prepareHtmlWithValues(&configuration);
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    free(html);
    return ESP_OK;
}

/* URI handler function for POST /save */
esp_err_t save_post_handler(httpd_req_t *req) {
    char buf[1024];
    size_t recv_size = httpd_req_recv(req, buf, sizeof(buf));
    if (recv_size > 0) {
        buf[recv_size] = '\0';
        Configuration conf;
        sscanf(buf, "%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%d&%*[^=]=%[^&]&%*[^=]=%d&%*[^=]=%[^&]", 
                conf.wifi_ssid, conf.wifi_password, conf.wallet_address, conf.pool_password, conf.pool_url, &conf.pool_port, conf.blink_enabled, 
                &conf.blink_brightness, conf.lcd_on_start);
        storage_save(&conf);
        httpd_resp_send(req, "Data saved successfully!<br/><br/>Please reboot your board!", HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send(req, "Failed to save data!", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

httpd_uri_t root_uri = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = root_get_handler,
    .user_ctx = NULL
};

httpd_uri_t save_uri = {
    .uri      = "/save",
    .method   = HTTP_POST,
    .handler  = save_post_handler,
    .user_ctx = NULL
};

void accesspoint_setup() {
    ESP_LOGI(TAG_AP, "Setup");

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .max_connection = 4,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("leafminer"));
    ESP_ERROR_CHECK(mdns_instance_name_set("LeafMiner Access Point"));

    ESP_ERROR_CHECK(nvs_flash_init());

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &save_uri);
}

void accesspoint_loop() {
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

*/