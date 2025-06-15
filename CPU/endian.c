#include "endian.h"

// As the endian thingy is pretty bad i decided to fix it
// a SHALL BE THE LOWER BYTE
// b SHALL BE THE HIGHER BYTE
uint16_t gather16Bit(uint8_t a, uint8_t b)
{
    return (b << 8) | a;
}
