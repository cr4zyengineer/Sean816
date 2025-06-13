#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "header.h"
#include "../memory.h"
#include "../cpu.h"

extern uint8_t mem[UINT16_MAX];
uint16_t offset = MEMORY_MAPPED_IO_REGION_SIZE;

void endianswapper(uint16_t *src, uint16_t *dest)
{
    uint8_t *relbuf = (uint8_t*)src;
    uint8_t *endianswap = (uint8_t*)dest;
    endianswap[0] = relbuf[1];
    endianswap[1] = relbuf[0];
}

void binload(const char *path)
{
    // Open file
    int fd = open(path, O_RDONLY);
    if(fd == -1)
    {
        printf("Error: Couldnt open binary at %s\n", path);
        return;
    }

    // Read its content slowly
    struct stat fileStat;
    fstat(fd, &fileStat);

    // Now we read loop
    uint8_t *vbuf = &mem[offset];
    ssize_t rbytes = read(fd, vbuf, fileStat.st_size);

    // Close file
    close(fd);

    // Get the header
    sean816_rom_executable_header_t *header = (sean816_rom_executable_header_t*)&mem[offset];

    // Check its magic
    if(header->magic != SEAN816_HEADER_MAGIC)
        return;

    // Relocate the offsets
    uint16_t *rloc_offsets = (uint16_t*)&mem[offset  + sizeof(sean816_rom_executable_header_t)];
    uint16_t binary_offset = ((sizeof(sean816_rom_executable_header_t)) + (sizeof(uint16_t) * header->reloc_count)) + offset;
    vbuf = &mem[binary_offset];
    for(uint16_t rloc = 0; rloc < header->reloc_count; rloc++)
    {
        uint16_t relocation_offset = rloc_offsets[rloc];
        uint16_t *relative_offset = (uint16_t*)(vbuf + relocation_offset);

        // As the Sean816 CPU has a completely different endian we need to cross both over and then again
        uint16_t roffset = 0;
        endianswapper(relative_offset, &roffset);
        roffset += binary_offset;
        endianswapper(&roffset, relative_offset);
    }

    uint16_t entry_point = binary_offset + header->entry;

    // Now were good to go?
    offset += rbytes;

    cpu_core_t *core = cpu_create_core();
    core->pc = entry_point;
    cpu_exec_core(core);
}
