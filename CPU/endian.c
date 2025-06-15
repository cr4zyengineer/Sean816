#include "endian.h"

// As the endian thingy is pretty bad i decided to fix it
uint16_t gather16Bit(uint8_t a, uint8_t b)
{
    return (a << 8) | b;
}
