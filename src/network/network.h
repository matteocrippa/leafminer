#ifndef NETWORK_H
#define NETWORK_H
#include <cJSON.h>
#include <string>
short network_getJob();
void network_send(std::string job_id, std::string extranonce2, std::string ntime, uint32_t nonce);
void network_listen();
void networkTaskFunction(void *pvParameters);
#endif // NETWORK_H