#ifndef NETWORK_H
#define NETWORK_H
#include <cJSON.h>
#include <string>
void network_getJob();
void network_send(const std::string &job_id, const std::string &extranonce2, const std::string &ntime, const uint32_t &nonce);
void network_task(void *pvParameters);
void network_loop();
#endif // NETWORK_H