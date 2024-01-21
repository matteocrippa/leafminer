#include "miner/miner.h"
#include "utils/utils.h"
#include "leafminer.h"
#include "current.h"
#include "utils/log.h"
#include "network/network.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif

char TAG_MINER[6] = "Miner";

void miner(uint32_t core)
{
    try
    {
        double diff_hash = 0;
        uint32_t winning_nonce = 0;
        uint8_t hash[SHA256M_BLOCK_SIZE];

        while (1)
        {

            if (current_job_is_valid == 0)
            {
                return;
            }

#if defined(ESP8266)
            ESP.wdtFeed();
#endif // ESP8266

            current_increment_hashes();

            if (!current_job->pickaxe(hash, winning_nonce))
            {
                continue;
            }

            diff_hash = diff_from_target(hash);
            if (diff_hash > current_getDifficulty())
            {
                break;
            }
        }

        current_update_hashrate();

#if defined(ESP32)
        l_info(TAG_MINER, "[%d] > Heap / Free heap / Min free heap: %d / %d / %d", core, ESP.getHeapSize(), ESP.getFreeHeap(), ESP.getMinFreeHeap());
#else
#if defined(HAS_LCD)
        screen_loop();
#endif // HAS_LCD
#endif

        l_debug(TAG_MINER, "[%d] > [%s] > 0x%.8x - diff %.12f", core, current_job->job_id.c_str(), winning_nonce, diff_hash);
        network_send(current_job->job_id, current_job->extranonce2, current_job->ntime, winning_nonce);
        if (core == 0)
        {
            network_listen();
        }

        current_setHighestDifficulty(diff_hash);

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