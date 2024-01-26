#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <Updater.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <Update.h>
#include <HTTPClient.h>
#endif // ESP8266
#include <cJSON.h>
#include <string>
#include "leafminer.h"
#include "autoupdate.h"
#include "model/configuration.h"
#include "utils/platform.h"
#include "utils/log.h"

const std::string AUTOUPDATE_URL = "https://raw.githubusercontent.com/matteocrippa/leafminer/feature/v0.0.5/version.json"; //"https://raw.githubusercontent.com/matteocrippa/leafminer/main/version.json";
const char TAG_AUTOUPDATE[] = "autoupdate";

#if defined(ESP8266_D)
std::string DEVICE = "esp8266";
#elif defined(GEEKMAGICCLOCK_SMALLTV)
std::string DEVICE = "geekmagic-smalltv";
#elif defined(ESP32_WROOM)
std::string DEVICE = "esp32";
#elif defined(ESP32_S2)
std::string DEVICE = "esp32-s2";
#elif defined(ESP32_S3)
std::string DEVICE = "esp32-s3";
#elif defined(LILYGO_T_S3)
std::string DEVICE = "lilygo-t-display-s3";
#else
std::string DEVICE = "unknown";
#endif

extern Configuration configuration;

void autoupdate()
{
    l_info(TAG_AUTOUPDATE, "Connecting to %s...", configuration.wifi_ssid.c_str());
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        WiFi.begin(configuration.wifi_ssid.c_str(), configuration.wifi_password.c_str());
        delay(800);
    }

    HTTPClient http;
    http.begin(AUTOUPDATE_URL.c_str());
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        std::string payload = http.getString().c_str();
        l_debug(TAG_AUTOUPDATE, "payload: %s", payload.c_str());

        cJSON *json = cJSON_Parse(payload.c_str());
        cJSON *versionItem = cJSON_GetObjectItem(json, "current");

        // Check if the "version" field exists and is a string
        if (versionItem != NULL && cJSON_IsString(versionItem))
        {
            std::string version = cJSON_GetStringValue(versionItem);
            // Now you can safely use the 'version' string
            l_debug(TAG_AUTOUPDATE, "Version: %s", version.c_str());
            if (strcmp(version.c_str(), _VERSION) == 0)
            {
                l_debug(TAG_AUTOUPDATE, "No Updates, Version: %s", version.c_str());
                return;
            }
            else
            {
                l_debug(TAG_AUTOUPDATE, "New Version: %s", version.c_str());
                cJSON *url = cJSON_GetObjectItemCaseSensitive(json, "link");
                cJSON *device = cJSON_GetObjectItemCaseSensitive(json, "device");

                if (strcmp(device->valuestring, DEVICE.c_str()) != 0)
                {
                    l_error(TAG_AUTOUPDATE, "Device not supported: %s", device->valuestring);
                    return;
                }
                else
                {
                    l_debug(TAG_AUTOUPDATE, "Device supported: %s", device->valuestring);
                    l_debug(TAG_AUTOUPDATE, "Downloading: %s", url->valuestring);

                    http.begin(url->valuestring);
                    int httpCode = http.GET();
                    if (httpCode == HTTP_CODE_OK)
                    {
                        l_debug(TAG_AUTOUPDATE, "Downloaded: %d", http.getSize());
                        if (Update.begin(http.getSize()))
                        {
                            l_debug(TAG_AUTOUPDATE, "Begin Update");
                            size_t written = Update.writeStream(http.getStream());
                            if (written == http.getSize())
                            {
                                l_debug(TAG_AUTOUPDATE, "Written: %d", written);
                                if (Update.end())
                                {
                                    l_debug(TAG_AUTOUPDATE, "Update Success: %d", written);
                                    ESP.restart();
                                }
                                else
                                {
                                    l_error(TAG_AUTOUPDATE, "Update Failed");
                                }
                            }
                            else
                            {
                                l_error(TAG_AUTOUPDATE, "Written: %d", written);
                            }
                        }
                        else
                        {
                            l_error(TAG_AUTOUPDATE, "Begin Update Failed");
                        }
                    }
                    else
                    {
                        l_error(TAG_AUTOUPDATE, "httpCode: %d", httpCode);
                    }
                }
            }
        }
        else
        {
            // Handle the case where the "version" field is missing or not a string
            l_error(TAG_AUTOUPDATE, "Invalid or missing 'version' field in JSON");
            // You might want to return or exit the function, depending on your application flow
            return;
        }
    }
    else
    {
        l_error(TAG_AUTOUPDATE, "httpCode: %d", httpCode);
    }
}