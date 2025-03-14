#ifndef JOB_H
#define JOB_H

#include <Arduino.h>
#include <vector>
#include "leafminer.h"
#include "subscribe.h"
#include "notification.h"
#include "utils/utils.h"
#include "model/block.h"
#include "model/target.h"
#include "miner/sha256m.h"
#include "miner/nerdSHA256plus.h"
#include "utils/log.h"

class Job
{
public:
    Block block;
    Target target;
    std::string job_id;
    std::string extranonce2;
    std::string ntime;

    Job(const Notification &notification, const Subscribe &subscribe, double difficulty);

    uint8_t pickaxe(uint32_t core, uint8_t *hash, uint32_t &winning_nonce);

    void setStartNonce(uint32_t start_nonce);

private:
    void nextNonce(uint32_t core);
    void generateCoinbaseHash(const std::string &coinbase, std::string &coinbase_hash);
    void calculateMerkleRoot(const std::string &coinbase_hash, const std::vector<std::string> &merkle_branch, std::string &merkle_root);
    std::string generate_extra_nonce2(int extranonce2_size);

    nerdSHA256_context sha;
    char TAG_JOB[4] = "Job";
    double difficulty;
};

#endif
