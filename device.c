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
 * Headers
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "memory.h"

extern memory_io_mapping_t iomem[MEMORY_MAPPED_IO_REGION_SIZE];

/*
 * Device loader
 *
 */
void device_load_device(const char *path)
{
    // Opening the device
    void *handle = dlopen(path, RTLD_LAZY);

    if(!handle)
        return;

    // Getting the functions well need
    uint16_t* (*device_offset)(void) = dlsym(handle, "device_offset");

    // Load em!
    if(device_offset)
    {
        void (*device_read)(uint16_t addr, uint8_t *value) = dlsym(handle, "device_read");
        void (*device_write)(uint16_t addr, uint8_t value) = dlsym(handle, "device_write");
        if(!device_read || !device_write)
        {
            printf("Error: Device %s is not valid!", path);
            exit(1);
        }
        uint16_t *addr = device_offset();
        for(size_t i = 0; addr[i] != 0x0000; i++)
        {
            if(addr[i] >= MEMORY_MAPPED_IO_REGION_SIZE)
                return;
            iomem[addr[i]].isSet = true;
            iomem[addr[i]].rfunction = device_read;
            iomem[addr[i]].wfunction = device_write;
        }
    }

    // Initilize if the symbol does exist
    void (*device_init)(void) = dlsym(handle, "device_init");
    if(device_init)
        device_init();
}
