#include "job.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include "esp_random.h"
#endif
#include <climits>

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

Job::Job(const Notification &notification, const Subscribe &subscribe, double difficulty) : difficulty(difficulty)
{
    try
    {
        // Initialize variables
        const char *version = notification.version.c_str();
        const char *prevhash = notification.prevhash.c_str();
        const char *ntime_string = notification.ntime.c_str();
        const char *nbits = notification.nbits.c_str();

        job_id = notification.job_id;
        ntime = ntime_string;

        // Generate extranonce2
#ifndef UNIT_TEST
        extranonce2 = generate_extra_nonce2(subscribe.extranonce2_size);
#else
        extranonce2 = "00000002";
#endif

        // Calculate coinbase hash
        const std::string coinbase = notification.coinb1 + subscribe.extranonce1 + extranonce2 + notification.coinb2;
        std::string coinbase_hash;
        generateCoinbaseHash(coinbase, coinbase_hash);

        // Calculate merkle root
        std::string merkle_root;
        calculateMerkleRoot(coinbase_hash, notification.merkle_branch, merkle_root);

        // Populate block data
        block.version = strtoul(version, nullptr, 16);
        hexStringToByteArray(prevhash, block.previous_block);
        reverseBytesAndFlip(block.previous_block, 32);
        hexStringToByteArray(merkle_root.c_str(), block.merkle_root);
        block.ntime = strtoul(ntime.c_str(), nullptr, 16);
        block.nbits = strtoul(nbits, nullptr, 16);
        block.nonce = 0;

        // Calculate target
        target.calculate(nbits);

        // Initialize SHA context
        nerd_mids(&sha, reinterpret_cast<unsigned char *>(&block));
    }
    catch (...)
    {
        l_error(TAG_JOB, "Exception occurred.");
    }
}

std::string Job::generate_extra_nonce2(int extranonce2_size)
{
    try
    {
        // Generate a random number between 0 and INT_MAX
#if defined(ESP8266)
        randomSeed(analogRead(A0));
        uint32_t randomValue = random();
#else
        uint32_t randomValue = esp_random();
#endif
        l_info(TAG_JOB, "Random value: %u", randomValue);

        // Convert the random number to a hex string
        char hexString[9]; // Enough to hold a 32-bit integer in hex (including null terminator)
        snprintf(hexString, sizeof(hexString), "%08X", randomValue);

        l_info(TAG_JOB, "Hex value: %s", hexString);

        return std::string(hexString);
    }
    catch (...)
    {
        l_error(TAG_JOB, "Exception occurred.");
        return ""; // Return empty string in case of error
    }
}

void Job::generateCoinbaseHash(const std::string &coinbase, std::string &coinbase_hash)
{
    try
    {
        const size_t len = coinbase.length();
        uint8_t coinbaseBytes[len / 2];
        hexStringToByteArray(coinbase.c_str(), coinbaseBytes);
        uint8_t hash[SHA256M_BLOCK_SIZE];
        sha256_double(coinbaseBytes, len / 2, hash);
        coinbase_hash = byteArrayToHexString(hash, SHA256M_BLOCK_SIZE);
        l_debug(TAG_JOB, "Coinbase hash: %s", coinbase_hash.c_str());
    }
    catch (...)
    {
        l_error(TAG_JOB, "Exception occurred.");
    }
}

void Job::calculateMerkleRoot(const std::string &coinbase_hash, const std::vector<std::string> &merkle_branch, std::string &merkle_root)
{
    try
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
        l_debug(TAG_JOB, "Merkle root: %s", merkle_root.c_str());
    }
    catch (...)
    {
        l_error(TAG_JOB, "Exception occurred.");
    }
}
