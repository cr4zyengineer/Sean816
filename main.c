/*
 * MIT License
 *
 * Copyright (c) 2025 Cr4zy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Main symbol
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ROM/loader.h"
#include "device.h"
#include "cpu.h"

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
