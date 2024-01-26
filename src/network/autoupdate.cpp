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
#include "utils/platform.h"
#include "utils/log.h"

const std::string AUTOUPDATE_URL = "https://raw.githubusercontent.com/matteocrippa/leafminer/main/version.json";
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

void autoupdate() {
    HTTPClient http;
    http.begin(AUTOUPDATE_URL.c_str());
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) {
        std::string payload = http.getString().c_str();
        l_debug(TAG_AUTOUPDATE, "payload: %s", payload.c_str());

        cJSON *json = cJSON_Parse(payload.c_str());
        cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");

        if (strcmp(version->valuestring, _VERSION) == 0) {
            l_debug(TAG_AUTOUPDATE, "No Updates, Version: %s", version->valuestring);
            return;
        } else {
            l_debug(TAG_AUTOUPDATE, "New Version: %s", version->valuestring);
            cJSON *url = cJSON_GetObjectItemCaseSensitive(json, "link");
            cJSON *device = cJSON_GetObjectItemCaseSensitive(json, "device");

            if(strcmp(device->valuestring, DEVICE.c_str()) != 0) {
                l_error(TAG_AUTOUPDATE, "Device not supported: %s", device->valuestring);
                return;
            } else {
                l_debug(TAG_AUTOUPDATE, "Device supported: %s", device->valuestring);
                l_debug(TAG_AUTOUPDATE, "Downloading: %s", url->valuestring);

                http.begin(url->valuestring);
                int httpCode = http.GET();
                if(httpCode == HTTP_CODE_OK) {
                    l_debug(TAG_AUTOUPDATE, "Downloaded: %d", http.getSize());
                    if (Update.begin(http.getSize())) {
                        l_debug(TAG_AUTOUPDATE, "Begin Update");
                        size_t written = Update.writeStream(http.getStream());
                        if (written == http.getSize()) {
                            l_debug(TAG_AUTOUPDATE, "Written: %d", written);
                            if (Update.end()) {
                                l_debug(TAG_AUTOUPDATE, "Update Success: %d", written);
                                ESP.restart();
                            } else {
                                l_error(TAG_AUTOUPDATE, "Update Failed");
                            }
                        } else {
                            l_error(TAG_AUTOUPDATE, "Written: %d", written);
                        }
                    } else {
                        l_error(TAG_AUTOUPDATE, "Begin Update Failed");
                    }
                } else {
                    l_error(TAG_AUTOUPDATE, "httpCode: %d", httpCode);
                }
            }
        }


    } else {
        l_error(TAG_AUTOUPDATE, "httpCode: %d", httpCode);
    }
}