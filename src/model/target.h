#ifndef TARGET_H
#define TARGET_H

#include <stdio.h>

class Target
{
public:
    uint8_t value[32] = {};

    /**
     * Calculates the target value based on the provided nbits string.
     * The nbits string is converted to a 32-bit integer, and the exponent and mantissa are extracted from it.
     * The target value is then calculated based on the exponent and mantissa, and stored in the 'value' array.
     * If the conversion fails or the nbits value is invalid, an error message is printed and the function returns.
     *
     * @param nbits The nbits string to calculate the target value from.
     */
    void calculate(const char *nbits)
    {
        // Convert the nbits string to a 32-bit integer.
        char *endPtr;
        uint32_t bits_value = strtoul(nbits, &endPtr, 16);

        // Check for conversion errors
        if (*endPtr != '\0' || bits_value == 0)
        {
            // Handle the error (print a message, set a default value, etc.).
            Serial.println("Error: Invalid nbits value");
            return;
        }

        // Extract the exponent and mantissa from bits_value.
        uint32_t exp = bits_value >> EXPONENT_SHIFT;
        uint32_t mant = bits_value & MANTISSA_MASK;

        // Calculate the shift value.
        uint32_t shift = 8 * (exp - 3);

        // Calculate the byte and bit positions.
        uint32_t sb = shift / 8;
        uint32_t rb = shift % 8;

        // Calculate the target value based on the exponent and mantissa.
        value[sb] = (mant << rb);
        value[sb + 1] = (mant >> (8 - rb));
        value[sb + 2] = (mant >> (16 - rb));
        value[sb + 3] = (mant >> (24 - rb));
    }

private:
    static const uint32_t EXPONENT_SHIFT = 24;
    static const uint32_t MANTISSA_MASK = 0xffffff;
};

#endif