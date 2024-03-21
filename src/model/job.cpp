#include "job.h"
#if defined(ESP8266)
#include "TridentTD_ESP_TrueRandom.h"
#else
#include "esp_random.h"
#endif
#include <climits>

Job::Job(const Notification &notification, const Subscribe &subscribe, double difficulty) : difficulty(difficulty)
{
    const char *version = notification.version.c_str();
    const char *prevhash = notification.prevhash.c_str();
    const char *ntime_string = notification.ntime.c_str();
    const char *nbits = notification.nbits.c_str();

    job_id = notification.job_id;
    ntime = ntime_string;

#ifndef UNIT_TEST
    extranonce2 = generate_extra_nonce2(subscribe.extranonce2_size);
#else
    extranonce2 = "00000002";
#endif
    const std::string coinbase = notification.coinb1 + subscribe.extranonce1 + extranonce2 + notification.coinb2;

    std::string coinbase_hash;
    generateCoinbaseHash(coinbase, coinbase_hash);

    std::string merkle_root;
    calculateMerkleRoot(coinbase_hash, notification.merkle_branch, merkle_root);

    block.version = strtoul(version, nullptr, 16);
    hexStringToByteArray(prevhash, block.previous_block);
    reverseBytesAndFlip(block.previous_block, 32);

    hexStringToByteArray(merkle_root.c_str(), block.merkle_root);
    block.ntime = strtoul(ntime.c_str(), nullptr, 16);
    block.nbits = strtoul(nbits, nullptr, 16);
    block.nonce = 0;

    target.calculate(nbits);

    nerd_mids(&sha, reinterpret_cast<unsigned char *>(&block));
}

uint8_t Job::pickaxe(uint32_t core, uint8_t *hash, uint32_t &winning_nonce)
{
    nextNonce(core);
    winning_nonce = block.nonce;
    return nerd_sha256d(&sha, reinterpret_cast<unsigned char *>(&block) + 64, hash);
}

void Job::setStartNonce(uint32_t start_nonce)
{
    block.nonce = start_nonce;
}

void Job::nextNonce(uint32_t core)
{
    block.nonce += (core % 2 == 0) ? 1 : 2;
}

void Job::generateCoinbaseHash(const std::string &coinbase, std::string &coinbase_hash)
{
    const size_t len = coinbase.length();
    uint8_t coinbaseBytes[len / 2];
    hexStringToByteArray(coinbase.c_str(), coinbaseBytes);
    uint8_t hash[SHA256M_BLOCK_SIZE];
    sha256_double(coinbaseBytes, len / 2, hash);
    coinbase_hash = byteArrayToHexString(hash, SHA256M_BLOCK_SIZE);
}

void Job::calculateMerkleRoot(const std::string &coinbase_hash, const std::vector<std::string> &merkle_branch, std::string &merkle_root)
{
    uint8_t hash[SHA256M_BLOCK_SIZE] = {};
    hexStringToByteArray(coinbase_hash.c_str(), hash);

    for (const auto &branch : merkle_branch)
    {
        uint8_t merkle_branch_bin[32];
        hexStringToByteArray(branch.c_str(), merkle_branch_bin);

        uint8_t merkle_concatenated[SHA256M_BLOCK_SIZE * 2];
        for (size_t j = 0; j < 32; j++)
        {
            merkle_concatenated[j] = hash[j];
            merkle_concatenated[32 + j] = merkle_branch_bin[j];
        }

        sha256_double(merkle_concatenated, sizeof(merkle_concatenated), hash);
    }

    merkle_root = byteArrayToHexString(hash, SHA256M_BLOCK_SIZE);
}

std::string Job::generate_extra_nonce2(int extranonce2_size)
{
// Generate a random number between 0 and INT_MAX
#if defined(ESP8266)
    uint32_t randomValue = esp.random(0, INT_MAX);
#else
    uint32_t randomValue = esp_random();
#endif
    l_info(TAG_JOB, "Random value: %u", randomValue);

    // Calculate the required length of the hex string
    int hexStringLength = 2 * extranonce2_size + 1; // 2 characters per byte + 1 for null terminator

    // Allocate memory for the extranonce2 string
    char *extranonce2 = new char[hexStringLength];

    // Convert the random number to a hex string
    snprintf(extranonce2, hexStringLength, "%u", randomValue);
    while (static_cast<int>(strlen(extranonce2)) < hexStringLength - 1)
    {
        char temp[hexStringLength];
        snprintf(temp, hexStringLength, "0%s", extranonce2);
        strcpy(extranonce2, temp);
    }
    return std::string(extranonce2);
}
