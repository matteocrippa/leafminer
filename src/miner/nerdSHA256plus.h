/************************************************************************************
*   written by: Bitmaker
*   based on: Blockstream Jade shaLib
*   thanks to @LarryBitcoin

*   Description:

*   NerdSha256plus is a custom C implementation of sha256d based on Blockstream Jade
    code https://github.com/Blockstream/Jade

    The folowing file can be used on any ESP32 implementation using both cores

*************************************************************************************/
#ifndef nerdSHA256plus_H_
#define nerdSHA256plus_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <Arduino.h>

#include "utils/platform.h"

#define NERD_SHA256_BLOCK_SIZE 64
#define NERD_BITCOIN_BLOCK_SIZE 80
#define NERD_JOB_BLOCK_SIZE 16

struct nerdSHA256_context
{
    uint8_t buffer[NERD_SHA256_BLOCK_SIZE];
    uint32_t digest[8];
};

/* Calculate midstate */
RAM_ATTR void nerd_mids(nerdSHA256_context *midstate, uint8_t dataIn[NERD_SHA256_BLOCK_SIZE]);
RAM_ATTR uint8_t nerd_sha256d(nerdSHA256_context *midstate, uint8_t dataIn[NERD_JOB_BLOCK_SIZE], uint8_t doubleHash[NERD_SHA256_BLOCK_SIZE]);

#endif