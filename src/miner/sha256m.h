#ifndef SHA256M_H
#define SHA256M_H

#include <stdint.h>
#include <stddef.h>

#define SHA256M_BLOCK_SIZE 32
#define SHA256M_BUFFER_SIZE 64

void sha256_double(const uint8_t *msg, size_t len, uint8_t output[SHA256M_BUFFER_SIZE]);
#endif