#include "accesspoint.h"
#include "utils/log.h"
#include "model/configuration.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <Updater.h>
typedef ESP8266WebServer WebServer;
#else
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Update.h>
#endif // ESP8266
#include "storage/storage.h"
// #include "html/setup.h"
// #include "html/ota.h"

extern Configuration configuration;
char TAG_AP[] = "AccessPoint";
char AP_SSID[] = "LEAFMINER";
char AP_DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

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
    std::string html = ""; // html_setup;
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
    return html;
}

#if defined(ESP32)
void handleUpdate()
{
    static UpdateClass update;

    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!update.begin(UPDATE_SIZE_UNKNOWN))
        {
            update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (update.end(true))
        {
            Serial.printf("Update Success: %uB\n", upload.totalSize);
            server.send(200, "text/html", "Update success. Rebooting...");
            delay(1000);
            ESP.restart();
        }
        else
        {
            update.printError(Serial);
            server.send(500, "text/plain", "Update failed");
        }
    }
}
#else
void handleUpdate()
{
    // TODO: Implement OTA for ESP8266
}
#endif

void accesspoint_webserver()
{
    // TODO: remove
    String html_ota = "";

    server.on("/", HTTP_GET, [](){ 
        std::string html = prepareHtmlWithValues(configuration);
        server.send(200, "text/html", html.c_str()); 
    });

    // server.on("/ota", HTTP_GET, [html_ota](){ 
    //     server.send(200, "text/html", html_ota.c_str()); 
    // });

    // server.on("/upload", HTTP_POST, handleUpdate);

    server.on("/save", HTTP_POST, [](){
        Configuration conf = Configuration();
        conf.wifi_ssid = server.arg("wifi_ssid").c_str();
        conf.wifi_password = server.arg("wifi_password").c_str();
        conf.wallet_address = server.arg("wallet_address").c_str();
        conf.pool_password = server.arg("pool_password").c_str();
        conf.pool_url = server.arg("pool_url").c_str();
        conf.pool_port = server.arg("pool_port").toInt();
        conf.blink_enabled = server.arg("blink_enabled").c_str();
        conf.blink_brightness = server.arg("blink_brightness").toInt();
        conf.lcd_on_start = server.arg("lcd_on_start").c_str();
        storage_save(conf);

        server.send(200, "text/html", "<html><body>Data saved successfully!<br/><br/>Please reboot your board!</body></html>"); 
    });

    server.onNotFound([](){ 
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "redirecting..."); 
    });

    server.begin();
    l_debug(TAG_AP, "AP Webserver - Started");
}

void accesspoint_setup()
{
    l_debug(TAG_AP, "Setup");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(AP_DNS_PORT, "*", WiFi.softAPIP());
    accesspoint_webserver();
}

void accesspoint_loop()
{
    dnsServer.processNextRequest();
    server.handleClient();
    delay(100);
}