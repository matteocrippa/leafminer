#include "accesspoint.h"
#include "utils/log.h"
#include "model/configuration.h"
#include <ESPAsyncWebServer.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <Updater.h>
#else
#include <WiFi.h>
#include <DNSServer.h>
#include <Update.h>
#endif // ESP8266
#include "storage/storage.h"
#include "html/setup.h"
#include "html/ota.h"

extern Configuration configuration;
char TAG_AP[] = "AccessPoint";
char AP_SSID[] = "LEAFMINER";
char AP_DNS_PORT = 53;

DNSServer dnsServer;
AsyncWebServer server(80);

void replacePattern(std::string &html, const std::string &pattern, const std::string &replacement)
{
    size_t pos = html.find(pattern);

    while (pos != std::string::npos)
    {
        html.replace(pos, pattern.length(), replacement);
        pos = html.find(pattern, pos + replacement.length());
    }
}

std::string prepareHtmlWithValues(const Configuration &configuration)
{
    std::string html = html_setup;
    replacePattern(html, "{{wifi_ssid}}", configuration.wifi_ssid);
    replacePattern(html, "{{wifi_password}}", configuration.wifi_password);
    replacePattern(html, "{{wallet_address}}", configuration.wallet_address);
    replacePattern(html, "{{pool_password}}", configuration.pool_password);
    replacePattern(html, "{{pool_url}}", configuration.pool_url);
    replacePattern(html, "{{pool_port}}", std::to_string(configuration.pool_port));
    replacePattern(html, "{{blink_brightness}}", std::to_string(configuration.blink_brightness));
    bool is_blink_on = strcmp(configuration.blink_enabled.c_str(), "on") == 0;
    replacePattern(html, "{{blink_enabled_on}}", is_blink_on ? "checked=\"checked\"" : "");
    replacePattern(html, "{{blink_enabled_off}}", !is_blink_on ? "checked=\"checked\"" : "");
    bool is_lcd_on = strcmp(configuration.lcd_on_start.c_str(), "on") == 0;
    replacePattern(html, "{{lcd_on_start_on}}", is_lcd_on ? "checked=\"checked\"" : "");
    replacePattern(html, "{{lcd_on_start_off}}", !is_lcd_on ? "checked=\"checked\"" : "");
    bool is_autoupdate_on = strcmp(configuration.auto_update.c_str(), "on") == 0;
    replacePattern(html, "{{auto_update_on}}", is_autoupdate_on ? "checked=\"checked\"" : "");
    replacePattern(html, "{{auto_update_off}}", !is_autoupdate_on ? "checked=\"checked\"" : "");
    return html;
}

void accesspoint_print_error()
{
#if defined(ESP32)
    std::string error = Update.errorString();
#else
    std::string error = Update.getErrorString().c_str();
#endif
    l_error(TAG_AP, "Update Failed: %s", error.c_str());
}

void accesspoint_webserver()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String html = String(prepareHtmlWithValues(configuration).c_str());
        request->send(200, "text/html", html); });

    server.on("/ota", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", String(html_ota)); });

    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200, "text/html", "Uploading firmware...<br/><br/>Please wait...<br/><br/>Device will reboot automatically after update is completed.<br/><br/>"); },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            if (!index)
            {
                l_debug(TAG_AP, "Update Start: %s", filename.c_str());
                if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
                {
                    accesspoint_print_error();
                }
            }
            if (!Update.hasError())
            {
                if (Update.write(data, len) != len)
                {
                    accesspoint_print_error();
                }
            }
            if (final)
            {
                if (Update.end(true))
                {
                    l_info(TAG_AP, "Update success. Please manually reboot...");
                    delay(1000);
                    ESP.restart();
                }
                else
                {
                    accesspoint_print_error();
                }
            }
        });

    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        Configuration conf = Configuration();
        conf.wifi_ssid = request->arg("wifi_ssid").c_str();
        conf.wifi_password = request->arg("wifi_password").c_str();
        conf.wallet_address = request->arg("wallet_address").c_str();
        conf.pool_password = request->arg("pool_password").c_str();
        conf.pool_url = request->arg("pool_url").c_str();
        conf.pool_port = request->arg("pool_port").toInt();
        conf.blink_enabled = request->arg("blink_enabled").c_str();
        conf.blink_brightness = request->arg("blink_brightness").toInt();
        conf.lcd_on_start = request->arg("lcd_on_start").c_str();
        conf.auto_update = request->arg("auto_update").c_str();
        storage_save(conf);

        request->send(200, "text/html", "<html><body>Data saved successfully!<br/><br/>Please reboot your board!</body></html>"); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->redirect("/"); });

    server.begin();
    l_info(TAG_AP, "Webserver Started");
}

uint64_t accesspoint_uptime = 0;

void accesspoint_setup()
{
    l_debug(TAG_AP, "Setup");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(AP_DNS_PORT, "*", WiFi.softAPIP());
    accesspoint_webserver();
    accesspoint_uptime = millis();
}

void accesspoint_loop()
{
    dnsServer.processNextRequest();
    delay(100);
    if (millis() - accesspoint_uptime > 300000)
    {
        l_debug(TAG_AP, "Stop AP and reboot");
        ESP.restart();
    }
}