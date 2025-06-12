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
uint8_t mem[UINT16_MAX];
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
    if(addr < MEMORY_MAPPED_IO_REGION_SIZE)
        memory_io_read(addr, value);
    else
        *value = mem[addr];
}

void memory_write(uint16_t addr,
                  uint8_t value)
{
    if(addr < MEMORY_MAPPED_IO_REGION_SIZE)
        memory_io_write(addr, value);
    else
        mem[addr] = value;
}

void memory_buffer_intl_exec(uint8_t buffer[UINT16_MAX - MEMORY_MAPPED_IO_REGION_SIZE])
{
    for(size_t i = 0; i < (UINT16_MAX - MEMORY_MAPPED_IO_REGION_SIZE); i++)
        mem[MEMORY_MAPPED_IO_REGION_SIZE + i] = buffer[i];
}

void memory_load_binary(const char *filePath)
{
    // Open file
    int fd = open(filePath, O_RDONLY);

    // Read its content slowly
    struct stat fileStat;
    fstat(fd, &fileStat);

    // Now we read loop
    uint8_t *vbuf = &mem[MEMORY_MAPPED_IO_REGION_SIZE];
    read(fd, vbuf, fileStat.st_size);

    // Close file
    close(fd);
}
