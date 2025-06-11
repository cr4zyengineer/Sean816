/*
 * Header
 *
 */
#include <unistd.h>
#include <stdint.h>
#include <termios.h>

/*
 * NOTE: Define this to get the offset, note: you have to nullterminate it!
 */
static uint16_t offsets[3] = {
    0x00C0,
    0x0000
};

uint16_t* device_offset(void)
{
    return offsets;
}

/*
 *
 */
void device_read(uint16_t addr, uint8_t *value)
{
    read(STDIN_FILENO, value, 1);
}

void device_write(uint16_t addr, uint8_t value)
{
    write(STDOUT_FILENO, &value, 1);
}

/*
 * Initilizer
 */
void device_init(void)
{
    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}
