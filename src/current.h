#ifndef CURRENT_H
#define CURRENT_H

#include "model/job.h"
#include "model/subscribe.h"
#include "model/notification.h"
#include "model/configuration.h"

extern Job *current_job;
extern Job *current_job_next;
extern uint16_t current_job_is_valid;

const uint32_t current_get_job_processed();
const bool current_hasJob();
void current_setJob(const Notification &notification);
const char *current_getJobId();
const char *current_getUptime();
void current_setSubscribe(Subscribe *subscribe);
const char *current_getSessionId();
void current_setDifficulty(double difficulty);
const double current_getDifficulty();
void current_increment_block_found();
const uint32_t current_get_block_found();
void current_set_hashrate(const uint64_t &time, const uint64_t &hashes);
const double current_get_hashrate();
void current_setHighestDifficulty(double difficulty);
const double current_getHighestDifficulty();
void current_increment_hash_accepted();
const uint32_t current_get_hash_accepted();
void current_increment_hash_rejected();
const uint32_t current_get_hash_rejected();
void current_increment_processedJob();

// ESP32
#if defined(ESP32)
extern SemaphoreHandle_t current_mutex;
extern char TAG_CURRENT[8];

#define CURRENT_SEMAPHORE(callback)                             \
    do                                                          \
    {                                                           \
        if (xSemaphoreTake(current_mutex, portMAX_DELAY))       \
        {                                                       \
            l_debug(TAG_CURRENT, "@ Semaphore taken");          \
            callback;                                           \
            xSemaphoreGive(current_mutex);                      \
            l_debug(TAG_CURRENT, "@ Semaphore given");          \
        }                                                       \
        else                                                    \
        {                                                       \
            l_error(TAG_CURRENT, "@!! Semaphore Error taking"); \
        }                                                       \
    } while (0)

void current_semaphoreInit();

#endif // ESP32

#endif // CURRENT_H
