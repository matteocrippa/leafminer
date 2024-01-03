#ifndef JOB_H
#define JOB_H

#include <Arduino.h>
#include <vector>
#include <memory>

#include "leafminer.h"
#include "subscribe.h"
#include "notification.h"
#include "utils/utils.h"
#include "model/block.h"
#include "model/target.h"
#include "miner/sha256m.h"
#include "miner/nerdSHA256plus.h"
#include "utils/log.h"

class Job {
public:
    Block block;
    Target target;
    std::string job_id;
    std::string extranonce2 = "00000002";
    std::string ntime;

    // Constructor
    Job(const Notification &notification, const Subscribe &subscribe, double difficulty);

    // Destructor
    ~Job();

    uint8_t pickaxe(uint8_t *hash, uint32_t &winning_nonce);

    void setStartNonce(uint32_t start_nonce);

private:
    // Private member functions
    void nextNonce();
    void generateCoinbaseHash(const std::string &coinbase, std::string &coinbase_hash);
    void calculateMerkleRoot(const std::string &coinbase_hash, const std::vector<std::string> &merkle_branch, std::string &merkle_root);

    nerdSHA256_context sha;
    std::string TAG_JOB = "Job";
    double difficulty;
};
#endif
