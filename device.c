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
