#include <vector>
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif // ESP8266
#include "model/configuration.h"
#include "network.h"
#include "utils/log.h"
#include "leafminer.h"
#include "current.h"
#include "model/configuration.h"

WiFiClient client = WiFiClient();
char TAG_NETWORK[8] = "Network";
uint64_t id = 0;
uint64_t requestJobId = 0;
uint8_t isRequestingJob = 0;
uint32_t authorizeId = 0;
uint8_t isAuthorized = 0;
Configuration configuration = Configuration(WIFI_SSID, WIFI_PASSWORD, ADDRESS, POOL_PASSWORD, POOL_URL, POOL_PORT);

#define NETWORK_BUFFER_SIZE 2048
#define NETWORK_TIMEOUT 1000 * 60

/**
 * @brief Generates the next ID for the network.
 *
 * This function returns the next available ID for the network. If the current ID is equal to UINT64_MAX,
 * the function wraps around and returns 1. Otherwise, it increments the current ID by 1 and returns the result.
 *
 * @return The next ID for the network.
 */
uint64_t nextId()
{
    return (id == UINT64_MAX) ? 1 : ++id;
}

/**
 * Checks if the device is connected to the network.
 *
 * @note This function requires the configuration to be set.
 */
void isConnected()
{

    if (WiFi.status() == WL_CONNECTED && client.connected())
    {
        return;
    }

    // check if we are already connected to WiFi
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        l_info(TAG_NETWORK, "Connecting to %s...", configuration.wifi_ssid.c_str());
        WiFi.begin(configuration.wifi_ssid.c_str(), configuration.wifi_password.c_str());
        delay(2000);
    }

    // and we are connected to the host
    while (!client.connect(configuration.pool_url.c_str(), configuration.pool_port))
    {
        delay(2000);
        l_debug(TAG_NETWORK, "Connecting to host %s...", configuration.pool_url.c_str());
    }
}

/**
 * Sends a request to the server with the specified payload.
 *
 * @param payload The payload to send to the server.
 */
void request(const char *payload)
{
    client.print(payload);
    l_info(TAG_NETWORK, ">>> %s", payload);
}

/**
 * Authorizes the network connection by sending a request with the appropriate payload.
 */
void authorize()
{
    char payload[1024];
    uint64_t next_id = nextId();
    isAuthorized = 0;
    authorizeId = next_id;
    sprintf(payload, "{\"id\":%llu,\"method\":\"mining.authorize\",\"params\":[\"%s\",\"%s\"]}\n", next_id, configuration.address.c_str(), configuration.password.c_str());
    request(payload);
}

/**
 * Subscribes to the mining service.
 * Generates a payload with the subscription details and sends it as a request.
 */
void subscribe()
{
    char payload[1024];
    sprintf(payload, "{\"id\":%llu,\"method\":\"mining.subscribe\",\"params\":[\"LeafMiner/%s\", null]}\n", nextId(), _VERSION);
    request(payload);
}

/**
 * Suggests the mining difficulty for the network.
 * This function generates a payload string with the necessary data and sends it as a request.
 */
void difficulty()
{
    char payload[1024];
    sprintf(payload, "{\"id\":%llu,\"method\":\"mining.suggest_difficulty\",\"params\":[%f]}\n", nextId(), DIFFICULTY);
    request(payload);
}

/**
 * Determines the response type based on the provided JSON document.
 *
 * @param doc The JSON document to analyze.
 * @return The response type as a const char*.
 *         Possible values are "subscribe", the value of the "method" key,
 *         "mining.submit" if the "result" key is true, "mining.submit.fail" if the "result" key is false,
 *         or "unknown" if none of the above conditions are met.
 */
const char *responseType(cJSON *json)
{
    const cJSON *result = cJSON_GetObjectItem(json, "result");
    if (result != NULL && cJSON_IsArray(result) && cJSON_GetArraySize(result) > 0)
    {
        const cJSON *item0 = cJSON_GetArrayItem(result, 0);
        if (item0 != NULL && cJSON_IsArray(item0) && cJSON_GetArraySize(item0) > 0)
        {
            const cJSON *item00 = cJSON_GetArrayItem(item0, 0);
            if (item00 != NULL && cJSON_IsArray(item00) && cJSON_GetArraySize(item00) > 0)
            {
                return "subscribe";
            }
        }
    }
    else if (cJSON_HasObjectItem(json, "method"))
    {
        return cJSON_GetStringValue(cJSON_GetObjectItem(json, "method"));
    }
    else if (cJSON_HasObjectItem(json, "result"))
    {
        const cJSON *result = cJSON_GetObjectItem(json, "result");
        if (authorizeId == cJSON_GetNumberValue(cJSON_GetObjectItem(json, "id")))
        {
            return "authorized";
        }
        if (cJSON_IsTrue(result))
        {
            return "mining.submit";
        }
        else
        {
            // we consider a fail when the error code is 21
            // aka "Job not found"
            if (cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetObjectItem(json, "error"), 0)) == 21)
            {
                return "mining.submit.fail";
            }
            else if (cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetObjectItem(json, "error"), 0)) == 23)
            {
                return "mining.submit.difficulty_too_low";
            }
            else
            {
                return "mining.submit";
            }
        }
    }

    return "unknown";
}

/**
 * @brief Handles the response received from the network.
 *
 * This function parses the response JSON and performs different actions based on the response type.
 * The response type determines how the response data is processed and stored.
 *
 */
void response(std::string r)
{
    cJSON *json = cJSON_Parse(r.c_str());
    const char *type = responseType(json);
    l_info(TAG_NETWORK, "<<< [%s] %s", type, r.c_str());

    if (strcmp(type, "subscribe") == 0)
    {
        const cJSON *result = cJSON_GetObjectItem(json, "result");
        if (cJSON_IsArray(result) && cJSON_IsArray(cJSON_GetArrayItem(result, 0)) &&
            cJSON_IsArray(cJSON_GetArrayItem(cJSON_GetArrayItem(result, 0), 0)))
        {
            const cJSON *subscribeIdJson = cJSON_GetArrayItem(cJSON_GetArrayItem(cJSON_GetArrayItem(result, 0), 0), 1);
            const cJSON *extranonce1Json = cJSON_GetArrayItem(result, 1);
            const cJSON *extranonce2SizeJson = cJSON_GetArrayItem(result, 2);

            if (cJSON_IsString(subscribeIdJson) && cJSON_IsString(extranonce1Json) && cJSON_IsNumber(extranonce2SizeJson))
            {
                std::string subscribeId = subscribeIdJson->valuestring;
                std::string extranonce1 = extranonce1Json->valuestring;
                int extranonce2_size = extranonce2SizeJson->valueint;
                Subscribe *subscribe = new Subscribe(subscribeId, extranonce1, extranonce2_size);
                current_setSubscribe(subscribe);
            }
        }
    }
    else if (strcmp(type, "mining.notify") == 0)
    {

        cJSON *params = cJSON_GetObjectItem(json, "params");
        std::string job_id = cJSON_GetArrayItem(params, 0)->valuestring;

        // fail fast check if job_id is the same as the current job
        if (current_hasJob() && strcmp(current_job->job_id.c_str(), job_id.c_str()) == 0)
        {
            l_error(TAG_NETWORK, "Job is the same as the current one");
            return;
        }

        if (cJSON_IsArray(params) && cJSON_GetArraySize(params) == 9)
        {
            std::string prevhash = cJSON_GetArrayItem(params, 1)->valuestring;
            std::string coinb1 = cJSON_GetArrayItem(params, 2)->valuestring;
            std::string coinb2 = cJSON_GetArrayItem(params, 3)->valuestring;
            cJSON *merkle_branch = cJSON_GetArrayItem(params, 4);
            std::string version = cJSON_GetArrayItem(params, 5)->valuestring;
            std::string nbits = cJSON_GetArrayItem(params, 6)->valuestring;
            std::string ntime = cJSON_GetArrayItem(params, 7)->valuestring;
            bool clean_jobs = cJSON_GetArrayItem(params, 8)->valueint == 1;

            std::vector<std::string> merkleBranchStrings;
            int merkleBranchSize = cJSON_GetArraySize(merkle_branch);
            for (int i = 0; i < merkleBranchSize; ++i)
            {
                merkleBranchStrings.push_back(cJSON_GetArrayItem(merkle_branch, i)->valuestring);
            }

            current_setJob(Notification(job_id, prevhash, coinb1, coinb2, merkleBranchStrings, version, nbits, ntime, clean_jobs));
            isRequestingJob = 0;
        }
    }
    else if (strcmp(type, "mining.set_difficulty") == 0)
    {
        const cJSON *paramsArray = cJSON_GetObjectItem(json, "params");
        if (cJSON_IsArray(paramsArray) && cJSON_GetArraySize(paramsArray) == 1)
        {
            const cJSON *difficultyItem = cJSON_GetArrayItem(paramsArray, 0);
            if (cJSON_IsNumber(difficultyItem))
            {
                double diff = difficultyItem->valuedouble;
                current_setDifficulty(diff);
                l_debug(TAG_NETWORK, "Difficulty set to: %.10f", diff);
            }
        }
    }
    else if (strcmp(type, "authorized") == 0)
    {
        l_info(TAG_NETWORK, "Authorized");
        isAuthorized = 1;
    }
    else if (strcmp(type, "mining.submit") == 0)
    {
        l_info(TAG_NETWORK, "Share accepted");
        current_increment_hash_accepted();
    }
    else if (strcmp(type, "mining.submit.difficulty_too_low") == 0)
    {
        l_error(TAG_NETWORK, "Share rejected due to low difficulty");
        current_increment_hash_rejected();
    }
    else if (strcmp(type, "mining.submit.fail") == 0)
    {
        l_error(TAG_NETWORK, "Share rejected");

        // prevent the current from requesting a new job, being old responses
        if ((uint64_t)cJSON_GetObjectItem(json, "id")->valueint < requestJobId)
        {
            l_error(TAG_NETWORK, "Late responses, skip them");
        }
        else
        {
            current_job_is_valid = 0;
            if (current_job_next != nullptr)
            {
                current_job = current_job_next;
                current_job_next = nullptr;
                current_job_is_valid = 1;
                l_debug(TAG_NETWORK, "Job (next): %s ready to be mined", current_job->job_id.c_str());
                current_increment_processedJob();
            }
            else
            {
                network_getJob();
            }
            current_increment_hash_rejected();
        }
    }
    else
    {
        l_error(TAG_NETWORK, "Unknown response type: %s", type);
    }
    cJSON_Delete(json);
    r.clear();
}

void network_getJob()
{
    if (current_job_is_valid == 1)
    {
        l_info(TAG_NETWORK, "Already has a job and don't need a new one");
        return;
    }

    if (isRequestingJob == 1)
    {
        l_info(TAG_NETWORK, "Already requesting a job");
        return;
    }

    isRequestingJob = 1;

    isConnected();

    if (current_getSessionId() == nullptr)
    {
        subscribe();
        authorize();
        difficulty();
    }

    requestJobId = nextId();
}

void network_send(const std::string &job_id, const std::string &extranonce2, const std::string &ntime, const uint32_t &nonce)
{
    char payload[256];
    snprintf(payload, sizeof(payload), "{\"id\":%llu,\"method\":\"mining.submit\",\"params\":[\"%s\",\"%s\",\"%s\",\"%s\",\"%08x\"]}\n", nextId(), configuration.address.c_str(), job_id.c_str(), extranonce2.c_str(), ntime.c_str(), nonce);
    request(payload);
}

void network_task(void *pvParameters)
{
    while (true)
    {
        network_loop();
    }
}

void network_loop() {
       char data[NETWORK_BUFFER_SIZE];
     int len = client.readBytesUntil('\n', data, sizeof(data) - 1);
        data[len] = '\0';

        // if empty, skip it
        if(data[0] != '\0') {
            response(data);
        }

#if defined(ESP32)
        vTaskDelay(1333 / portTICK_PERIOD_MS);
#else
        delay(1333);
#endif // ESP32
}