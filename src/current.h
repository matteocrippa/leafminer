#ifndef CURRENT_H
#define CURRENT_H

#include "model/job.h"
#include "model/subscribe.h"
#include "model/notification.h"
#include "model/configuration.h"

extern Job *current_job;
#if defined(ESP32)
extern Job *current_job_next;
#endif
extern uint16_t current_job_is_valid;

void current_setJob(const Notification &notification);
const char *current_getJobId();
const char *current_getUptime();
void current_setSubscribe(Subscribe *subscribe);
const char *current_getSessionId();
void current_resetSession();
void current_setDifficulty(double difficulty);
const double current_getDifficulty();
void current_increment_block_found();
const uint32_t current_get_block_found();
const double current_get_hashrate();
void current_setHighestDifficulty(double difficulty);
const double current_getHighestDifficulty();
void current_increment_hash_accepted();
const uint32_t current_get_hash_accepted();
void current_increment_hash_rejected();
const uint32_t current_get_hash_rejected();
void current_increment_processedJob();
void current_increment_hashes();
void current_update_hashrate();
void current_check_stale();
bool current_hasJob();

// Declaration for ESP32 specific task function
#if defined(ESP32)
void currentTaskFunction(void *pvParameters);
#endif

#endif
