#include <Arduino.h>
#include <climits>
#include "current.h"
#include "utils/log.h"
#include "screen/screen.h"

char TAG_CURRENT[8] = "Current";

Job *current_job = nullptr;
Job *current_job_next = nullptr;
uint16_t current_job_is_valid = 0;
Subscribe *current_subscribe = nullptr;
uint64_t current_job_processed = 0;
double current_difficulty = UINT_MAX;
double current_difficulty_highest = 0.0;
uint64_t current_block_found = 0;
uint64_t current_hash_accepted = 0;
uint64_t current_hash_rejected = 0;
uint32_t current_hashes = 0;
uint64_t current_hashes_time = 0;
double current_hashrate = 0;
uint64_t current_uptime = millis();
uint64_t current_last_hash = millis();

bool current_hasJob()
{
    return current_job != nullptr;
}

void current_increment_processedJob()
{
    current_job_processed++;
}

void current_setJob(const Notification &notification)
{
    if (current_subscribe == nullptr)
    {
        l_error(TAG_CURRENT, "Subscribe object is null");
        return;
    }

    if (notification.clean_jobs)
    {
        current_job_is_valid = 0;
        if (current_job != nullptr)
        {
            l_debug(TAG_CURRENT, "Job: %s is cleaned and replaced with %s", current_job->job_id.c_str(), notification.job_id.c_str());
        }
    }

    if (current_job_is_valid == 1)
    {
#if CORES > 1
        current_job_next = new Job(notification, *current_subscribe, current_difficulty);
        l_info(TAG_CURRENT, "Job: %s queued", current_job_next->job_id.c_str());
#endif
        return;
    }

    current_job = new Job(notification, *current_subscribe, current_difficulty);
    current_job_is_valid = 1;
    current_increment_processedJob();
    l_info(TAG_CURRENT, "Job: %s ready to be mined", current_job->job_id.c_str());
}

const char *current_getJobId()
{
    return (current_job != nullptr) ? current_job->job_id.c_str() : "--";
}

const char *current_getUptime()
{
    uint64_t time = millis() - current_uptime;
    uint64_t seconds = time / 1000;
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;
    uint64_t days = hours / 24;

    static char uptimeString[22];
    snprintf(uptimeString, sizeof(uptimeString), "%lldd %lldh %lldm", days, hours % 24, minutes % 60);

    return uptimeString;
}

void current_resetSession()
{
    current_subscribe = nullptr;
}

void current_setSubscribe(Subscribe *subscribe)
{
    l_info(TAG_CURRENT, "New session id: %s", subscribe->id.c_str());
    current_subscribe = subscribe;
}

const char *current_getSessionId()
{
    return (current_subscribe != nullptr) ? current_subscribe->id.c_str() : nullptr;
}

void current_setDifficulty(double difficulty)
{
    l_info(TAG_CURRENT, "New difficulty: %.12f", difficulty);
    current_difficulty = difficulty;
}

const double current_getDifficulty()
{
    return current_difficulty;
}

void current_increment_block_found()
{
    current_block_found++;
    l_info(TAG_CURRENT, "Block found: %d", current_block_found);
}

const uint32_t current_get_block_found()
{
    return current_block_found;
}

const double current_get_hashrate()
{
    return current_hashrate;
}

void current_setHighestDifficulty(double difficulty)
{
    if (difficulty > current_difficulty_highest)
    {
        current_difficulty_highest = difficulty;
        l_info(TAG_CURRENT, "New highest hashed difficulty: %.12f", difficulty);
    }
}

const double current_getHighestDifficulty()
{
    return current_difficulty_highest;
}

void current_increment_hash_accepted()
{
    current_hash_accepted++;
    current_last_hash = millis();
    l_info(TAG_CURRENT, "Hash accepted: %d", current_hash_accepted);
}

const uint32_t current_get_hash_accepted()
{
    return current_hash_accepted;
}

void current_increment_hash_rejected()
{
    current_hash_rejected++;
    current_last_hash = millis();
    l_info(TAG_CURRENT, "Hash rejected: %d", current_hash_rejected);
}

const uint32_t current_get_hash_rejected()
{
    return current_hash_rejected;
}

void current_increment_hashes()
{
    if (current_hashes_time == 0)
    {
        current_hashes_time = millis();
    }
    current_hashes++;
}

void current_update_hashrate()
{
    if (millis() - current_hashes_time > 1000)
    {
        current_hashrate = (current_hashes / ((millis() - current_hashes_time) / 1000.0)) / 1000.0; // KH/s
        l_debug(TAG_CURRENT, "Hashrate: %.2f kH/s", current_hashrate);
#if defined(HAS_LCD)
        screen_loop();
#endif
        current_hashes = 0;
        current_hashes_time = millis();
    }
}

void current_check_stale()
{
    if (millis() - current_last_hash > 200000)
    {
        l_error(TAG_CURRENT, "No hash received in the last 3 minutes. Restarting...");
        ESP.restart();
    }
}

#if defined(ESP32)
#define CURRENT_STALE_TIMEOUT 50000
void currentTaskFunction(void *pvParameters)
{
    while (1)
    {
        current_check_stale();
        vTaskDelay(CURRENT_STALE_TIMEOUT / portTICK_PERIOD_MS);
    }
}
#endif
