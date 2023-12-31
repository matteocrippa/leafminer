#include "job.h"


// Constructor
Job::Job(const Notification &notification, const Subscribe &subscribe, double difficulty) : difficulty(difficulty) {
    const char *version = notification.version.c_str();
    const char *prevhash = notification.prevhash.c_str();
    const char *ntime_string = notification.ntime.c_str();
    const char *nbits = notification.nbits.c_str();
    uint8_t merkle_branch_size = notification.merkle_branch.size();
    std::vector<std::string> merkle_branch(merkle_branch_size);
    for (uint8_t i = 0; i < merkle_branch_size; i++) {
        merkle_branch[i] = notification.merkle_branch[i].c_str();
    }

    job_id = notification.job_id;
    ntime = ntime_string;

    std::string coinbase = notification.coinb1 + subscribe.extranonce1 + extranonce2 + notification.coinb2;
    l_info(TAG_JOB.c_str(), "[%s] - Coinbase: %s", job_id.c_str(), coinbase.c_str());

    std::string coinbase_hash;
    generateCoinbaseHash(coinbase, coinbase_hash);

    std::string merkle_root;
    calculateMerkleRoot(coinbase_hash, merkle_branch, merkle_root);

    block.version = strtoul(version, NULL, 16);
    hexStringToByteArray(prevhash, block.previous_block);
    reverseBytesAndFlip(block.previous_block, 32);

    hexStringToByteArray(merkle_root.c_str(), block.merkle_root);
    block.ntime = strtoul(ntime.c_str(), NULL, 16);
    block.nbits = strtoul(nbits, NULL, 16);
    block.nonce = 0;

    target.calculate(nbits);

    nerd_mids(&sha, (unsigned char *)&block);
}

// Destructor
Job::~Job() {
    // No need to manually release resources for std::string members
}

uint8_t Job::pickaxe(uint8_t *hash, uint32_t &winning_nonce) {
    nextNonce();
    winning_nonce = block.nonce;
    uint8_t is_valid = nerd_sha256d(&sha, (unsigned char *)&block + 64, hash);
    return is_valid;
}

void Job::setStartNonce(uint32_t start_nonce) {
    block.nonce = start_nonce;
}

void Job::nextNonce() {
    block.nonce++;
}

void Job::generateCoinbaseHash(const std::string &coinbase, std::string &coinbase_hash) {
    // Convert hex string to byte array
    size_t len = coinbase.length();
    uint8_t coinbaseBytes[len / 2];
    hexStringToByteArray(coinbase.c_str(), coinbaseBytes);

    // Calculate SHA256 hash
    uint8_t hash[SHA256M_BLOCK_SIZE];
    sha256_double(coinbaseBytes, len / 2, hash);

    // Convert binary hash to hex string
    coinbase_hash = byteArrayToHexString(hash, SHA256M_BLOCK_SIZE);
    l_info(TAG_JOB.c_str(), "[%s] - Coinbase hash: %s", job_id.c_str(), coinbase_hash.c_str());
}

void Job::calculateMerkleRoot(const std::string &coinbase_hash, const std::vector<std::string> &merkle_branch, std::string &merkle_root) {
    uint8_t hash[SHA256M_BLOCK_SIZE] = {};
    hexStringToByteArray(coinbase_hash.c_str(), hash);

    for (size_t i = 0; i < merkle_branch.size(); i++) {
        uint8_t merkle_branch_bin[32];
        hexStringToByteArray(merkle_branch[i].c_str(), merkle_branch_bin);

        l_info(TAG_JOB.c_str(), "[%s] - Merkle Branch [%d]: %s", job_id.c_str(), i, merkle_branch[i].c_str());

        uint8_t merkle_concatenated[SHA256M_BLOCK_SIZE * 2];

        for (size_t j = 0; j < 32; j++) {
            merkle_concatenated[j] = hash[j];
            merkle_concatenated[32 + j] = merkle_branch_bin[j];
        }

        sha256_double(merkle_concatenated, sizeof(merkle_concatenated), hash);
    }

    merkle_root = byteArrayToHexString(hash, SHA256M_BLOCK_SIZE);
}
