#include "miner/miner.h"
#include "utils/utils.h"
#include "leafminer.h"
#include "current.h"
#include "utils/log.h"
#include "network/network.h"
#if defined(HAS_LCD)
#include "screen/screen.h"
#endif

char TAG_MINER[] = "Miner";

void miner(uint32_t core)
{
    double diff_hash = 0;
    uint32_t winning_nonce = 0;
    uint8_t hash[SHA256M_BLOCK_SIZE];
    uint32_t local_hashes = 0;
    // Improved nonce distribution: each core gets its own stride and offset
    static bool is_initialized[2] = {false, false}; // Track init per core
    static uint32_t nonce_offsets[2] = {0, 0};      // Random offset per core
    #if defined(ESP32)
        const uint32_t miner_count = (CORE == 2) ? 2 : 1;
    #else
        const uint32_t miner_count = 1; // ESP8266 is single core
    #endif
    const uint32_t nonce_stride = miner_count;
    
    // Initialize random offset for this core on first call
    if (!is_initialized[core]) {
    #if defined(ESP8266)
        nonce_offsets[core] = random() & 0xFFFFFF; // 24-bit random offset
    #else
        nonce_offsets[core] = esp_random() & 0xFFFFFF; // 24-bit random offset
    #endif
        is_initialized[core] = true;
    }
    
    uint32_t wdt_counter = 0; // Counter for WDT feeding
    const uint32_t batch_size = 64; // Process 64 hashes before checking for work updates

    while (current_job_is_valid && current_job)
    {
        // Process a batch of hashes before checking for work updates
        // This reduces latency in getting new work but increases stale work risk slightly
        uint32_t batch_start = local_hashes;
        uint32_t hashes_in_batch = 0;
        
        while (hashes_in_batch < batch_size && current_job_is_valid && current_job)
        {
#if defined(ESP8266)
            // Feed WDT less frequently to reduce overhead (every 256 hashes)
            if (++wdt_counter >= 256) {
                ESP.wdtFeed();
                wdt_counter = 0;
            }
#endif

            local_hashes++;
            hashes_in_batch++;
            if (!current_job->pickaxe((core * nonce_stride) + nonce_offsets[core], hash, winning_nonce))
            {
                // Flush hash count and check hashrate every 256 nonces
                if ((local_hashes & 0xFF) == 0)
                {
                    current_batch_hashes(local_hashes);
                    local_hashes = 0;
                    current_update_hashrate();
                }
                continue;
            }

            // Flush remaining hashes before evaluating difficulty
            current_batch_hashes(local_hashes);
            local_hashes = 0;

            diff_hash = diff_from_target(hash);
            if (diff_hash > current_getDifficulty())
            {
                l_debug(TAG_MINER, "[%d] > Hash %.12f > %.12f", core, diff_hash, current_getDifficulty());
                break;
            }
            current_update_hashrate();
        }
        
        // If we found a valid hash that meets difficulty, break out
        if (diff_hash > current_getDifficulty())
        {
            break;
        }
        
        // If job became invalid during batch processing, break out
        if (!current_job_is_valid || !current_job)
        {
            break;
        }
    }
    
    // Flush any remaining hashes
    if (local_hashes > 0)
    {
        current_batch_hashes(local_hashes);
    }
#if defined(HAS_LCD)
    screen_loop();
#endif // HAS_LCD

    if(!current_job_is_valid)
        return;
    l_info(TAG_MINER, "[%d] > [%s] > 0x%.8x - diff %.12f", core, current_job->job_id.c_str(), winning_nonce, diff_hash);
    network_send(current_job->job_id, current_job->extranonce2, current_job->ntime, winning_nonce);

    current_setHighestDifficulty(diff_hash);
    if(!current_job)
        return;
    if (littleEndianCompare(hash, current_job->target.value, 32) < 0)
    {
        l_info(TAG_MINER, "[%d] > Found block - 0x%.8x", core, current_job->block.nonce);
        current_increment_block_found();
    }
}

#if defined(ESP32)
void mineTaskFunction(void *pvParameters)
{
    uint32_t core = (uint32_t)pvParameters;
    while (1)
    {
        if (current_job_is_valid && current_job)
            miner(core);
        vTaskDelay(100); // Add a small delay to prevent tight loop
    }
}
#endif
