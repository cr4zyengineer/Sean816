/*
 * Header
 *
 */
#include <unistd.h>
#include <stdint.h>

/*
 * NOTE: Define this to get the offset, note: you have to nullterminate it!
 */
static uint16_t offsets[3] = {
    0x0001,
    0x0000
};

uint16_t* device_offset(void)
{
    return offsets;
}

/*
 *
 */
uint8_t some_device_value = 0;
void device_read(uint16_t addr, uint8_t *value)
{
    *value = some_device_value;
}

void device_write(uint16_t addr, uint8_t value)
{
    some_device_value = value;
}

/*
 * Initilizer
 */
void device_init(void)
{
    some_device_value = 27;
}
