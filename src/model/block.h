#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

// Represents a block in a blockchain
#pragma pack(push, 1) // Set alignment to 1 byte
struct Block
{
    uint32_t version;           // Block version
    uint8_t previous_block[32]; // Previous block hash
    uint8_t merkle_root[32];    // Merkle root hash
    uint32_t ntime;             // Timestamp
    uint32_t nbits;             // Target difficulty
    uint32_t nonce;             // Nonce
};
#pragma pack(pop) // Reset alignment to default

#endif
