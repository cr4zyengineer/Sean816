/*
 * Main symbol
 *
 */
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include "memory.h"
#include "device.h"
#include "cpu.h"

void binload(const char *path);

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Usage: %s [-device <path to device>] <path/to/binary>\n", argv[0]);
        return 1;
    }

    // Load devices
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-device") == 0) {
            if (i + 1 < argc) {
                if(access(argv[i + 1], R_OK) == 0)
                {
                    device_load_device(argv[++i]);
                } else {
                    fprintf(stderr, "Error: device not found at %s\n", argv[i + 1]);
                    return 1;
                }
            } else {
                fprintf(stderr, "Error: -device option requires a path argument\n");
                return 1;
            }
        }
    }

    // Execute
    binload(argv[argc - 1]);

    return 0;
}
