#include <stdio.h>
#include <stddef.h>
#include "header.h"
#include "../memory.h"
#include "../cpu.h"

extern uint8_t mem[UINT16_MAX];

void endianswapper(uint16_t *src, uint16_t *dest)
{
    uint8_t *relbuf = (uint8_t*)src;
    uint8_t *endianswap = (uint8_t*)dest;
    endianswap[0] = relbuf[1];
    endianswap[1] = relbuf[0];
}

void binload(const char *path)
{
    // Load the binary
    memory_load_binary(path);

    // Get the header
    sean816_rom_executable_header_t *header = (sean816_rom_executable_header_t*)&mem[MEMORY_MAPPED_IO_REGION_SIZE];

    // Check its magic
    if(header->magic != SEAN816_HEADER_MAGIC)
        return;

    // Relocate the offsets
    uint8_t *vbuf = &mem[(MEMORY_MAPPED_IO_REGION_SIZE + sizeof(sean816_rom_executable_header_t)) + (sizeof(uint16_t) * header->reloc_count)];
    uint16_t *rloc_offsets = (uint16_t*)&mem[MEMORY_MAPPED_IO_REGION_SIZE  + sizeof(sean816_rom_executable_header_t)];
    for(uint16_t rloc = 0; rloc < header->reloc_count; rloc++)
    {
        uint16_t relocation_offset = rloc_offsets[rloc];
        uint16_t *relative_offset = (uint16_t*)(vbuf + relocation_offset);

        // As the Sean816 CPU has a completely different endian we need to cross both over and then again
        uint16_t roffset = 0;
        endianswapper(relative_offset, &roffset);
        roffset += (MEMORY_MAPPED_IO_REGION_SIZE + sizeof(sean816_rom_executable_header_t)) + (sizeof(uint16_t) * header->reloc_count);
        endianswapper(&roffset, relative_offset);
    }

    // Now were good to go?
    cpu_core_t *core = cpu_create_core();
    core->pc = (MEMORY_MAPPED_IO_REGION_SIZE + sizeof(sean816_rom_executable_header_t)) + (sizeof(uint16_t) * header->reloc_count);
    cpu_exec_core(core);
}
