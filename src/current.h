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
#if defined(ESP32)
void currentTaskFunction(void *pvParameters);
#endif
#endif