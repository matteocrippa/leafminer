#include <unity.h>
#include <Arduino.h>
#include <vector>
#include <cJSON.h>
#include "leafminer.h"
#include "model/target.h"
#include "model/block.h"
#include "model/job.h"
#include "model/subscribe.h"
#include "model/notification.h"
#include "utils/utils.h"
#include "miner/sha256m.h"
#include "miner/nerdSHA256plus.h"
#include "network/network.h"

void test_create_target(void)
{
    const char *nbits = "19015f53";
    Target target;
    target.calculate(nbits);

    const char *expected_target_string = "00000000000000015f5300000000000000000000000000000000000000000000";
    uint8_t expected_target[32];
    stringToLittleEndianBytes(expected_target_string, expected_target);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_target, target.value, sizeof(target.value));
}

void test_create_block_and_mine(void)
{
    const char *nbits = "19015f53";
    const char *ntime = "53058b35";
    const char *version = "00000002";
    const char *prev_block = "000000000000000117c80378b8da0e33559b5997f2ad55e2f7d18ec1975b9717";
    const char *merkle_root = "871714dcbae6c8193a2bb9b2a69fe1c0440399f38d94b3a0f1b447275a29978a";

    Block block;
    block.version = strtoul(version, NULL, 16);
    stringToLittleEndianBytes(prev_block, block.previous_block);
    stringToLittleEndianBytes(merkle_root, block.merkle_root);
    block.ntime = strtoul(ntime, NULL, 16);
    block.nbits = strtoul(nbits, NULL, 16);
    block.nonce = 856192328;

    char *block_header_string = (char *)malloc(161); // 2 * 80 + 1 for the null terminator
    for (int i = 0; i < 80; i++)
    {
        sprintf(block_header_string + (i * 2), "%02x", ((uint8_t *)&block)[i]);
    }

    const char *expected_hash = "0200000017975b97c18ed1f7e255adf297599b55330edab87803c81701000000000000008a97295a2747b4f1a0b3948df3990344c0e19fa6b2b92b3a19c8e6badc141787358b0553535f011948750833";

    TEST_ASSERT_EQUAL_STRING(expected_hash, block_header_string);

    Target target;
    target.calculate(nbits);
    uint8_t hash[SHA256M_BLOCK_SIZE];
    nerdSHA256_context sha;
    nerd_mids(&sha, (unsigned char *)&block);
    nerd_sha256d(&sha, (unsigned char *)&block + 64, hash);

    Serial.println("\nHash:");
    char hash_string[65];
    hexInverse(hash, 32, hash_string);
    Serial.println(hash_string);

    Serial.println("Target:");
    char target_string[65];
    hexInverse(target.value, 32, target_string);
    Serial.println(target_string);

    TEST_ASSERT_TRUE(littleEndianCompare(hash, target.value, 32) < 0);
}

void test_create_job()
{
    // https://bitcoin.stackexchange.com/questions/22929/full-example-data-for-scrypt-stratum-client

    std::vector<std::string> merkle_branch;
    merkle_branch.push_back("57351e8569cb9d036187a79fd1844fd930c1309efcd16c46af9bb9713b6ee734");
    merkle_branch.push_back("936ab9c33420f187acae660fcdb07ffdffa081273674f0f41e6ecc1347451d23");

    Notification *notification = new Notification("b3ba", "7dcf1304b04e79024066cd9481aa464e2fe17966e19edf6f33970e1fe0b60277", "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff270362f401062f503253482f049b8f175308", "0d2f7374726174756d506f6f6c2f000000000100868591052100001976a91431482118f1d7504daf1c001cbfaf91ad580d176d88ac00000000", merkle_branch, "00000002", "1b44dfdb", "53178f9b", true);

    Subscribe *subscribe = new Subscribe("ae6812eb4cd7735a302a8a9dd95cf71f", "f8002c90", 4);

    Job *job = new Job(*notification, *subscribe, 0);

    const char *expected_hash = "020000000413cf7d02794eb094cd66404e46aa816679e12f6fdf9ee11f0e97337702b6e00b1edc1ccf82d3214423fc68234f4946119e39df2cc2137e31ebc186191d54229b8f1753dbdf441b00000000";

    char *block_header_string = (char *)malloc(161); // 2 * 80 + 1 for the null terminator
    for (int i = 0; i < 80; i++)
    {
        sprintf(block_header_string + (i * 2), "%02x", ((uint8_t *)&job->block)[i]);
    }

    TEST_ASSERT_EQUAL_STRING(expected_hash, block_header_string);
}

void test_double_sha256m()
{
    const char *msg = "0200000017975b97c18ed1f7e255adf297599b55330edab87803c81701000000000000008a97295a2747b4f1a0b3948df3990344c0e19fa6b2b92b3a19c8e6badc141787358b0553535f011948750833";
    uint8_t msg_bytes[80];
    hexStringToByteArray(msg, msg_bytes);
    const char *expected_double_hash = "0000000000000000e067a478024addfecdc93628978aa52d91fabd4292982a50";

    uint8_t hash[SHA256M_BLOCK_SIZE];
    char hash_string[65];

    // Record the start time
    uint64_t startTime = micros();

    // Perform double SHA-256
    sha256_double(msg_bytes, 80, hash);

    // Record the end time
    uint64_t endTime = micros();

    // Calculate the elapsed time
    uint64_t elapsedTime = endTime - startTime;

    // Print the performance information
    Serial.print("Double SHA-256 Performance: ");
    Serial.print(elapsedTime);
    Serial.println(" microseconds");

    // Convert the hash to a string for comparison
    hexInverse(hash, 32, hash_string);
    TEST_ASSERT_EQUAL_STRING(expected_double_hash, hash_string);
}

void test_nerdminer()
{
    const char *msg = "0200000017975b97c18ed1f7e255adf297599b55330edab87803c81701000000000000008a97295a2747b4f1a0b3948df3990344c0e19fa6b2b92b3a19c8e6badc141787358b0553535f011948750833";
    uint8_t msg_bytes[80];
    hexStringToByteArray(msg, msg_bytes);

    const char *expected_hash = "0000000000000000e067a478024addfecdc93628978aa52d91fabd4292982a50";

    uint8_t hash[32];

    uint64_t startTime = micros();

    nerdSHA256_context sha;
    nerd_mids(&sha, msg_bytes);
    uint64_t endTime = micros();

    // Calculate the elapsed time
    uint64_t elapsedTime = endTime - startTime;
    Serial.print("NerdMiner Init: ");
    Serial.print(elapsedTime);
    Serial.println(" microseconds");

    // Record the start time
    startTime = micros();
    bool is_valid = nerd_sha256d(&sha, msg_bytes + 64, hash);
    // Record the end time
    endTime = micros();

    // Calculate the elapsed time
    elapsedTime = endTime - startTime;
    Serial.print("NerdMiner Performance: ");
    Serial.print(elapsedTime);
    Serial.println(" microseconds");

    char hash_string[65];
    hexInverse(hash, 32, hash_string);
    TEST_ASSERT_EQUAL_STRING(expected_hash, hash_string);

    TEST_ASSERT_TRUE(is_valid);
}

void test_performance_nerdminer()
{
    uint8_t blockheader[80] = {0};
    char *result = new char[256];
    nerdSHA256_context sha;
    uint8_t hash[32];

    for (int i = 0; i < 80; i++)
    {
        if (i < 10)
            blockheader[i] = 0;
        else
            blockheader[i] = 0xFF;
    }

    uint64_t startTime = micros();
    nerd_mids(&sha, blockheader);
    uint64_t elapsedTime = micros() - startTime;

    sprintf(result, "\nNERD - Midstate init: %lld microseconds\n", elapsedTime);

    startTime = micros();
    bool is_valid = true;
    for (size_t i = 0; i < 1000; i++)
    {
        is_valid = nerd_sha256d(&sha, blockheader + 64, hash);
    }
    elapsedTime = (micros() - startTime) / 1000;

    Serial.print(result);
    sprintf(result, "NERD - Midstate final: avg. ~%lld microseconds\n", elapsedTime);
    Serial.print(result);

    TEST_ASSERT_FALSE(is_valid);
}

void setup()
{
    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_create_block_and_mine);
    RUN_TEST(test_create_target);
    RUN_TEST(test_create_job);
    RUN_TEST(test_double_sha256m);
    RUN_TEST(test_nerdminer);

    // Performance Testing
    RUN_TEST(test_performance_nerdminer);

    UNITY_END();
}

void loop()
{
}
