#include "miner/miner.h"
#include "utils/utils.h"
#include "leafminer.h"
#include "current.h"
#include "utils/log.h"
#include "network/network.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif

MEM_ATTR uint64_t miner_hashes = 0;
MEM_ATTR uint64_t miner_time = 0;
char TAG_MINER[6] = "Miner";

#if defined(ESP32)
#define MINER_UPDATE_HASHES 330000
#else
#define MINER_UPDATE_HASHES 10000
#endif

void miner(uint32_t core)
{
    try
    {
        if (miner_time == 0)
        {
            miner_time = millis();
        }

        double diff_hash = 0;
        uint32_t winning_nonce = 0;
        uint8_t hash[SHA256M_BLOCK_SIZE];

        while (1)
        {

            miner_hashes++;

            if (current_job_is_valid == 0)
            {
                return;
            }

#if defined(ESP8266)
            ESP.wdtFeed();
#endif // ESP8266

            if (!current_job->pickaxe(hash, winning_nonce))
            {
                continue;
            }
            
            diff_hash = diff_from_target(hash);
            if (diff_hash > current_getDifficulty()) {
                break;
            }
        }

        current_set_hashrate(miner_time, miner_hashes);
        miner_hashes = 0;
        miner_time = millis();

#if defined(ESP32)
        l_info(TAG_MINER, "[%d] > Heap / Free heap / Min free heap: %d / %d / %d", core, ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMinFreeHeap());
        CURRENT_SEMAPHORE({ network_send(current_job->job_id, current_job->extranonce2, current_job->ntime, winning_nonce); });
#else
        network_send(current_job->job_id, current_job->extranonce2, current_job->ntime, winning_nonce);
#if defined(HAS_LCD)
        screen_loop();
#endif // HAS_LCD
#endif

        current_setHighestDifficulty(diff_hash);

        l_debug(TAG_MINER, "[%d] > [%s] > 0x%.8x - diff %.12f", core, current_job->job_id.c_str(), winning_nonce, diff_hash);

        if (littleEndianCompare(hash, current_job->target.value, 32) < 0)
        {
            l_debug(TAG_MINER, "[%d] > Found block - 0x%.8x", core, current_job->block.nonce);
            current_increment_block_found();
        }
    }
    catch (const std::exception &e)
    {
        l_error(TAG_MINER, "[%d] > Error: %s", core, e.what());
    }
}

#if defined(ESP32)
void mineTaskFunction(void *pvParameters)
{
    uint32_t core = (uint32_t)pvParameters;
    while (1)
    {
        miner(core);
        vTaskDelay(portTICK_PERIOD_MS);
    }
}
#endif