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

int main(int argc, char **argv)
{
    memory_load_binary("./Sean816Asm/test.bin");

    // Load hardcoded devices
    //device_load_device("Device/Template/TemplateDevice.so");
    device_load_device("Device/Serial/Serial.so");

    // Execute code
    cpu_core_t *core = cpu_create_core();
    cpu_exec_core(core);

    return 0;
}
