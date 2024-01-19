#include <Arduino.h>
#include <climits>
#if defined(ESP32)
#include <freertos/semphr.h>
#endif
#include "current.h"
#include "utils/log.h"

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

/**
 * @brief Increments the count of processed jobs.
 */
void current_increment_processedJob()
{
    current_job_processed++;
}

/**
 * Retrieves the value of the processed job.
 *
 * @return The value of the processed job.
 */
const uint32_t current_get_job_processed()
{
    return current_job_processed;
}

/**
 * @brief Checks if the object has a job assigned.
 *
 * @return true if the object has a job assigned, false otherwise.
 */
const bool current_hasJob()
{
    return current_job != nullptr;
}

/**
 * Sets the job for mining.
 *
 * @param notification The notification containing the job details.
 */
void current_setJob(const Notification &notification)
{
    if (current_subscribe == nullptr)
    {
        l_error(TAG_CURRENT, "Subscribe object is null");
        return;
    }
    if (notification.clean_jobs)
    {
        current_job_processed = 0;
        if (current_job != nullptr)
        {
          l_debug(TAG_CURRENT, "Job: %s is cleaned and replaced with %s", current_job->job_id.c_str(), notification.job_id.c_str());
        }
    }
    if (current_job_is_valid == 1)
    {
        current_job_next = new Job(notification, *current_subscribe, current_difficulty);
        l_debug(TAG_CURRENT, "Job: %s queued", current_job_next->job_id.c_str());
        return;
    }
    current_job = new Job(notification, *current_subscribe, current_difficulty);
    current_job_is_valid = 1;
    l_debug(TAG_CURRENT, "Job: %s ready to be mined", current_job->job_id.c_str());
    current_increment_processedJob();
}

/**
 * @brief Get the job ID.
 *
 * This function returns the job ID associated with the current object.
 * If the job is nullptr, it returns "--".
 *
 * @return char* The job ID.
 */
const char *current_getJobId()
{
    return (current_job != nullptr) ? current_job->job_id.c_str() : "--";
}

/**
 * @brief Get the current uptime in days, hours, minutes, and seconds.
 *
 * This function calculates the current uptime by subtracting the stored uptime value from the current millis value.
 * It then converts the time into days, hours, minutes, and seconds format and returns it as a string.
 *
 * @return const char* The current uptime in the format "dd hh mm ss".
 */
const char *current_getUptime()
{
    uint64_t time = millis() - current_uptime;
    uint64_t seconds = time / 1000;
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;
    uint64_t days = hours / 24;

    // Using a static buffer for the return string
    static char uptimeString[22];

    sprintf(uptimeString, "%lldd %lldh %lldm", days, hours % 24, minutes % 60);

    return uptimeString;
}

/**
 * Sets the Subscribe object for the Current class.
 *
 * @param subscribe A pointer to the Subscribe object.
 */
void current_setSubscribe(Subscribe *subscribe)
{
    l_info(TAG_CURRENT, "New session id: %s", subscribe->id.c_str());
    current_subscribe = subscribe;
}

/**
 * @brief Gets the session ID.
 *
 * This function returns the session ID if the 'subscribe' object is not null,
 * otherwise it returns nullptr.
 *
 * @return const char* The session ID or nullptr.
 */
const char *current_getSessionId()
{
    return (current_subscribe != nullptr) ? current_subscribe->id.c_str() : nullptr;
}

/**
 * @brief Sets the difficulty level.
 *
 * This function sets the difficulty level of the current object.
 *
 * @param difficulty The new difficulty level.
 */
void current_setDifficulty(double difficulty)
{
    l_info(TAG_CURRENT, "New difficulty: %.12f", difficulty);
    current_difficulty = difficulty;
}

/**
 * @brief Gets the difficulty of the current object.
 *
 * @return The difficulty value.
 */
const double current_getDifficulty()
{
    return current_difficulty;
}

/**
 * @brief Increments the value of the found_block variable by 1.
 */
void current_increment_block_found()
{
    current_block_found++;
    l_info(TAG_CURRENT, "Block found: %d", current_block_found);
}

/**
 * @brief Get the found block value.
 *
 * @return The value of the found block.
 */
const uint32_t current_get_block_found()
{
    return current_block_found;
}

/**
 * @brief Get the hashrate value.
 *
 * This function returns the current hashrate value.
 *
 * @return The hashrate value.
 */
const double current_get_hashrate()
{
    return current_hashrate;
}

/**
 * Sets the highest difficulty value.
 *
 * @param difficulty The new difficulty value.
 */
void current_setHighestDifficulty(double difficulty)
{
    if (difficulty > current_difficulty_highest)
    {
        current_difficulty_highest = difficulty;
        l_debug(TAG_CURRENT, "New highest hashed difficulty: %.12f", difficulty);
    }
}

/**
 * @brief Get the highest difficulty value.
 *
 * This function returns the highest difficulty value.
 *
 * @return The highest difficulty value.
 */
const double current_getHighestDifficulty()
{
    return current_difficulty_highest;
}

void current_increment_hash_accepted()
{
    current_hash_accepted++;
    l_info(TAG_CURRENT, "Hash accepted: %d", current_hash_accepted);
}

const uint32_t current_get_hash_accepted()
{
    return current_hash_accepted;
}

void current_increment_hash_rejected()
{
    current_hash_rejected++;
    l_info(TAG_CURRENT, "Hash rejected: %d", current_hash_rejected);
}

const uint32_t current_get_hash_rejected()
{
    return current_hash_rejected;
}

void current_increment_hashes() {
    if (current_hashes_time == 0) {
        current_hashes_time = millis();
    }
    current_hashes++;
}

void current_update_hashrate() {
    current_hashrate = (current_hashes / ((millis() - current_hashes_time) / 1000.0)) / 1000.0; // KH/s
    l_debug(TAG_CURRENT, "Hashrate: %.2f kH/s", current_hashrate);
    current_hashes = 0;
    current_hashes_time = millis();
}


#if defined(ESP32)

SemaphoreHandle_t current_mutex;

/**
 * @brief Initializes the current semaphore.
 *
 * This function creates a mutex semaphore using xSemaphoreCreateMutex() function.
 * If the semaphore creation fails, an error message is logged using l_error() function.
 */
void current_semaphoreInit()
{
    current_mutex = xSemaphoreCreateMutex();
    if (current_mutex == NULL)
    {
        l_error(TAG_CURRENT, "Error creating mutex");
    }
}

#endif