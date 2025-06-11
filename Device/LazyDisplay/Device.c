/*
 * Header
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>

/*
 * NOTE: Define this to get the offset, note: you have to nullterminate it!
 */
static uint16_t offsets[4] = {
    0x00E0,			// Set X position
    0x00E1,			// Set Y position
    0x00E2,			// Set Pixel state
    0x0000
};

uint16_t* device_offset(void)
{
    return offsets;
}

/*
 *
 */
uint8_t x = 0;
uint8_t y = 0;

int comfd = 0;

void draw(uint8_t isOn)
{
    uint8_t payload[3] = {x,y,isOn};
    write(comfd, payload, sizeof(uint8_t) * 3);
}

void device_read(uint16_t addr, uint8_t *value)
{
    *value = 0;
}

void device_write(uint16_t addr, uint8_t value)
{
    switch(addr)
    {
        case 0x00E0:
            x = value;
            break;
        case 0x00E1:
            y = value;
            break;
        default:
            draw(value);
            break;
    }
}

/*
 * Initilizer
 */
void device_init(void)
{
    x = 0;
    y = 0;

    pid_t pid = fork();

    if (pid == 0) {
        if (prctl(PR_SET_PDEATHSIG, SIGTERM) == -1) {
            perror("prctl");
            exit(EXIT_FAILURE);
        }

        execl("./Device/LazyDisplay/DisplayUI/DisplayUI", "DisplayUI");
        exit(1);
    } else if (pid > 0) {
        usleep(500000);
        comfd = open("com", O_WRONLY, 0777);
    } else {
        perror("fork failed");
        exit(1);
    }
}
