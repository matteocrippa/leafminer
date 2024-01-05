#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string>

// Constants for clarity
static constexpr double TRUEDIFFONE = 26959535291011309493156476344723991336010898738574164086137773096960.0;
static constexpr double BITS192 = 6277101735386680763835789423207666416102355444464034512896.0;
static constexpr double BITS128 = 340282366920938463463374607431768211456.0;
static constexpr double BITS64 = 18446744073709551616.0;

/**
 * Converts a hexadecimal character to its corresponding decimal value.
 *
 * @param ch The hexadecimal character to convert.
 * @return The decimal value of the hexadecimal character.
 */
static inline uint8_t hex(char ch)
{
    uint8_t r = (ch > 57) ? (ch - 55) : (ch - 48);
    return r & 0x0F;
}

/**
 * @brief Converts a hex string to a byte array.
 *
 * This function takes a null-terminated hex string and converts it into a byte array.
 * Each pair of characters in the hex string represents a single byte in the output array.
 * The output array must have enough space to store the converted bytes.
 *
 * @param hexString The null-terminated hex string to convert.
 * @param output The output byte array to store the converted bytes.
 */
static inline void hexStringToByteArray(const char *hexString, uint8_t *output)
{
    while (*hexString)
    {
        *output = (hex(*hexString++) << 4) | hex(*hexString++);
        output++;
    }
}

static inline std::string byteArrayToHexString(const byte *byteArray, size_t length)
{
    static const char hex_array[] = "0123456789abcdef";
    std::string result;

    for (size_t i = 0; i < length; i++)
    {
        byte value = byteArray[i];
        result += hex_array[value >> 4];
        result += hex_array[value & 0xF];
    }

    return result;
}

/**
 * Reverses the order of hexadecimal characters in the given array and stores the result in the output string.
 *
 * @param hex The input array containing hexadecimal characters.
 * @param len The length of the input array.
 * @param output The output string where the reversed hexadecimal characters will be stored.
 */
static void hexInverse(unsigned char *hex, size_t len, char *output)
{
    for (size_t i = len - 1; i < len; --i)
    {
        sprintf(output, "%02x", hex[i]);
        output += 2;
    }
}

/**
 * Converts a string to little-endian byte representation.
 *
 * @param in The input string to convert.
 * @param output The output buffer to store the converted bytes.
 */
static inline void stringToLittleEndianBytes(const char *in, uint8_t *output)
{
    size_t len = strlen(in);
    assert(len % 2 == 0);

    for (size_t s = 0, b = (len / 2 - 1); s < len; s += 2, --b)
    {
        output[b] = (unsigned char)(hex(in[s]) << 4) + hex(in[s + 1]);
    }
}

/**
 * Reverses the order of bytes in the given data array and flips each byte.
 *
 * @param data The data array to be reversed and flipped.
 * @param len The length of the data array.
 */
static void reverseBytesAndFlip(uint8_t *data, size_t len)
{
    for (unsigned int i = 0; i < len / 4; ++i)
    {
        uint8_t temp[4];
        for (int j = 0; j < 4; ++j)
        {
            temp[j] = data[i * 4 + j];
        }
        for (int j = 0; j < 4; ++j)
        {
            data[i * 4 + j] = temp[3 - j];
        }
    }
}

/**
 * Compares two byte arrays in little-endian order.
 *
 * @param a The first byte array to compare.
 * @param b The second byte array to compare.
 * @param byte_len The length of the byte arrays.
 * @return -1 if a is less than b, 1 if a is greater than b, 0 if they are equal.
 */
static int littleEndianCompare(const unsigned char *a, const unsigned char *b, size_t byte_len)
{
    for (size_t i = byte_len - 1; i < byte_len; --i)
    {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }
    return 0;
}

/**
 * @brief Converts a little-endian 256-bit value to a double.
 *
 * This function takes a pointer to a 32-byte array representing a little-endian 256-bit value,
 * and converts it to a double value. The conversion is performed by interpreting the bytes as
 * a sequence of four 64-bit values and multiplying each value by a corresponding constant.
 * The resulting values are then summed up to obtain the final double value.
 *
 * @param target Pointer to the little-endian 256-bit value.
 * @return The converted double value.
 */
static double littleEndian256ToDouble(const uint8_t *target)
{
    const uint64_t *data64;

    data64 = reinterpret_cast<const uint64_t *>(target + 24);
    double dcut64 = *data64 * BITS192;

    data64 = reinterpret_cast<const uint64_t *>(target + 16);
    dcut64 += *data64 * BITS128;

    data64 = reinterpret_cast<const uint64_t *>(target + 8);
    dcut64 += *data64 * BITS64;

    data64 = reinterpret_cast<const uint64_t *>(target);
    dcut64 += *data64;

    return dcut64;
}

/**
 * Calculates the difference from the target value based on the given hash.
 *
 * @param hash The hash value to calculate the difference from.
 * @return The difference from the target value.
 */
static double diff_from_target(const uint8_t *hash)
{
    double d64, s64, ds;
    d64 = TRUEDIFFONE;
    s64 = littleEndian256ToDouble(hash);
    ds = d64 / s64;

    return ds;
}

#endif // UTILS_H