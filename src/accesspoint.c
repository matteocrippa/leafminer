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
#include "configuration.h"
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "dhcpserver/dhcpserver.h"

#define TAG_AP "AccessPoint"
#define AP_SSID "LEAFMINER"
#define AP_DNS_PORT 53
#define AP_HTML_MAX_SIZE 1000

httpd_handle_t server = NULL;

extern Configuration configuration;

void replacePattern(char *html, const char *pattern, const char *replacement)
{
    int pos = strstr(html, pattern) - html;

    while (pos >= 0)
    {
        strcpy(html + pos, replacement);
        strcpy(html + pos + strlen(replacement), html + pos + strlen(pattern));
        pos = strstr(html + pos + strlen(replacement), pattern) - html;
    }
}

char *prepareHtmlWithValues(const Configuration *configuration)
{
    char *html = malloc(AP_HTML_MAX_SIZE * sizeof(char));
    if (html == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(html, html_setup);

    replacePattern(html, "{{wifi_ssid}}", configuration->wifi_ssid);
    replacePattern(html, "{{wifi_password}}", configuration->wifi_password);
    replacePattern(html, "{{wallet_address}}", configuration->wallet_address);
    replacePattern(html, "{{pool_password}}", configuration->pool_password);
    replacePattern(html, "{{pool_url}}", configuration->pool_url);

    char port_string[20];
    sprintf(port_string, "%d", configuration->pool_port);
    replacePattern(html, "{{pool_port}}", port_string);

    char brightness_string[20];
    sprintf(brightness_string, "%d", configuration->blink_brightness);
    replacePattern(html, "{{blink_brightness}}", brightness_string);

    const char *blink_enabled_check = strcmp(configuration->blink_enabled, "on") == 0 ? "checked=\"checked\"" : "";
    replacePattern(html, "{{blink_enabled_on}}", blink_enabled_check);

    const char *blink_disabled_check = strcmp(configuration->blink_enabled, "off") == 0 ? "checked=\"checked\"" : "";
    replacePattern(html, "{{blink_enabled_off}}", blink_disabled_check);

    const char *lcd_on_start_on_check = strcmp(configuration->lcd_on_start, "on") == 0 ? "checked=\"checked\"" : "";
    replacePattern(html, "{{lcd_on_start_on}}", lcd_on_start_on_check);

    const char *lcd_on_start_off_check = strcmp(configuration->lcd_on_start, "off") == 0 ? "checked=\"checked\"" : "";
    replacePattern(html, "{{lcd_on_start_off}}", lcd_on_start_off_check);

    return html;
}

esp_err_t accesspoint_upload_handler(httpd_req_t *req)
{
    httpd_resp_send(req, "Uploading firmware...<br/><br/>Please wait...<br/><br/>Device will reboot automatically after update is completed.<br/><br/>", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler function for GET / */
esp_err_t root_get_handler(httpd_req_t *req)
{
    char *html = prepareHtmlWithValues(&configuration);
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    free(html);
    return ESP_OK;
}

/* URI handler function for POST /save */
esp_err_t save_post_handler(httpd_req_t *req)
{
    char buf[1024];
    size_t recv_size = httpd_req_recv(req, buf, sizeof(buf));
    if (recv_size > 0)
    {
        buf[recv_size] = '\0';
        Configuration conf;
        sscanf(buf, "%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%[^&]&%*[^=]=%hd&%*[^=]=%[^&]&%*[^=]=%hd&%*[^=]=%[^&]",
               conf.wifi_ssid, conf.wifi_password, conf.wallet_address, conf.pool_password, conf.pool_url, &conf.pool_port, conf.blink_enabled,
               &conf.blink_brightness, conf.lcd_on_start);
        storage_save(&conf);
        httpd_resp_send(req, "Data saved successfully!<br/><br/>Please reboot your board!", HTTPD_RESP_USE_STRLEN);
    }
    else
    {
        httpd_resp_send(req, "Failed to save data!", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

/* URI handler function for GET /ota */
esp_err_t ota_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_ota, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void accesspoint_webserver(const Configuration *configuration)
{
    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler};

    httpd_uri_t ota_uri = {
        .uri = "/ota",
        .method = HTTP_GET,
        .handler = ota_handler};

    httpd_uri_t upload_uri = {
        .uri = "/upload",
        .method = HTTP_POST,
        .handler = accesspoint_upload_handler};

    httpd_uri_t save_uri = {
        .uri = "/save",
        .method = HTTP_POST,
        .handler = save_post_handler};

    // httpd_uri_t notfound_uri = {
    //     .uri = "/*",
    //     .method = HTTP_ANY,
    //     .handler = [](httpd_req_t *req)
    //     {
    //         httpd_resp_send(req, "", 0);
    //         return ESP_OK;
    //     }};

    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &ota_uri);
    httpd_register_uri_handler(server, &upload_uri);
    httpd_register_uri_handler(server, &save_uri);
    // httpd_register_uri_handler(server, &notfound_uri);
}

void accesspoint_setup()
{
    ESP_LOGI(TAG_AP, "Setup");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .max_connection = 1,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("leafminer"));
    ESP_ERROR_CHECK(mdns_instance_name_set("LeafMiner Access Point"));

    ESP_ERROR_CHECK(nvs_flash_init());

    esp_netif_dns_info_t dns;
    dns.ip.u_addr.ip4.addr = ESP_IP4TOADDR(192, 168, 4, 1);
    dns.ip.type = IPADDR_TYPE_V4;
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    accesspoint_webserver(&configuration);
}

// #define AP_IP_ADDR "192.168.4.1"
// #define AP_SUBNET_MASK "255.255.255.0"
// #define AP_DNS_ADDR "192.168.4.1"

// void wifi_init_softap()
// {
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // Create default event loop
//     esp_netif_create_default_wifi_ap();

//     // Initialize Wi-Fi config
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // Set static IP configuration
//     esp_netif_ip_info_t ip_info;
//     memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
//     IP4_ADDR(&ip_info.ip, 192, 168, 4, 1);
//     IP4_ADDR(&ip_info.gw, 192, 168, 4, 1);
//     IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
//     ESP_ERROR_CHECK(esp_netif_dhcps_stop(TCPIP_ADAPTER_IF_AP));
//     ESP_ERROR_CHECK(esp_netif_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info));
//     ESP_ERROR_CHECK(esp_netif_dhcps_start(TCPIP_ADAPTER_IF_AP));

//     // Set DNS server configuration
//     dns_server_t dns;
//     IP_ADDR4(&dns.ip, 192, 168, 4, 1);
//     ESP_ERROR_CHECK(esp_netif_dns_info_set(TCPIP_ADAPTER_IF_AP, ESP_NETIF_DNS_MAIN, &dns));

//     // Set Wi-Fi mode to AP
//     wifi_config_t wifi_config = {
//         .ap = {
//             .ssid = AP_SSID,
//             .ssid_len = strlen(AP_SSID),
//             .channel = 0,
//             .max_connection = 1,
//             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
//             .beacon_interval = 100},
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));

//     // Start Wi-Fi
//     ESP_ERROR_CHECK(esp_wifi_start());

//     // Register Wi-Fi event handler
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
// }

void accesspoint_loop()
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
}