/*
 * Headers
 *
 */
#include "memory.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * This holds the memory
 *
 * NOTE: 16bit maximum integer because its a 8bit cpu with 16bit addressing
 *
 */
uint8_t mem[MEMORY_SIZE];
memory_io_mapping_t iomem[MEMORY_MAPPED_IO_REGION_SIZE];

/*
 * IO Memory shit
 *
 */
void memory_io_set(uint16_t addr,
                   void *rfunction,
                   void *wfunction)
{
    if(addr >= MEMORY_MAPPED_IO_REGION_SIZE)
        return;
    iomem[addr].isSet = true;
    iomem[addr].rfunction = rfunction;
    iomem[addr].wfunction = wfunction;
}

/*
 * Read and write
 *
 */
void memory_read(uint16_t addr,
                 uint8_t *value)
{
    if(addr < MEMORY_MAPPED_IO_REGION_SIZE && iomem[addr].isSet)
        iomem[addr].rfunction(addr, value);
    else if(addr <= MEMORY_SIZE)
        *value = mem[addr];
    else
        *value = 0x00;
}

void memory_write(uint16_t addr,
                  uint8_t value)
{
    if(addr < MEMORY_MAPPED_IO_REGION_SIZE && iomem[addr].isSet)
        iomem[addr].wfunction(addr, value);
    else if(addr <= MEMORY_SIZE)
        mem[addr] = value;
}
