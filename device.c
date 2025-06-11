/*
 * Headers
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include "memory.h"

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
        uint16_t *addr = device_offset();
        void (*device_read)(uint8_t *value) = dlsym(handle, "device_read");
        void (*device_write)(uint8_t value) = dlsym(handle, "device_write");
        for(size_t i = 0; addr[i] != 0x0000; i++)
            memory_io_set(addr[i], device_read, device_write);
    }

    // Initilize if the symbol does exist
    void (*device_init)(void) = dlsym(handle, "device_init");
    if(device_init)
        device_init();
}
