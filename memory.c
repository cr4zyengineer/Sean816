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
    iomem[addr].type = MIO_SET;
    iomem[addr].rfunction = rfunction;
    iomem[addr].wfunction = wfunction;
}

void memory_io_read(uint16_t addr,
                    uint8_t *value)
{
    if(iomem[addr].type != MIO_SET) return;
    void (*read)(uint16_t addr, uint8_t *value) = iomem[addr].rfunction;
    read(addr, value);
}

void memory_io_write(uint16_t addr,
                     uint8_t value)
{
    if(iomem[addr].type != MIO_SET) return;
    void (*write)(uint16_t addr, uint8_t value) = iomem[addr].wfunction;
    write(addr, value);
}

/*
 * Read and write
 *
 */
void memory_read(uint16_t addr,
                 uint8_t *value)
{
    if(addr >= MEMORY_SIZE)
        *value = 0x00;
    else if(addr < MEMORY_MAPPED_IO_REGION_SIZE)
        memory_io_read(addr, value);
    else
        *value = mem[addr];
}

void memory_write(uint16_t addr,
                  uint8_t value)
{
    if(addr >= MEMORY_SIZE)
        return;
    else if(addr < MEMORY_MAPPED_IO_REGION_SIZE)
        memory_io_write(addr, value);
    else
        mem[addr] = value;
}
