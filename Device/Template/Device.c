/*
 * Header
 *
 */
#include <stdint.h>

/*
 * NOTE: Not defining one means its ignored, dont worry
 */
uint16_t device_offset(void)
{
    return 0x0000;
}

/*
 *
 */
void device_read(uint8_t *value)
{
    *value = 0xFF;
}

void device_write(uint8_t value)
{
    // Implement handling of writing
}
