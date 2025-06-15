#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "header.h"
#include "../memory.h"
#include "../cpu.h"

extern uint8_t mem[MEMORY_SIZE];
uint16_t offset = MEMORY_MAPPED_IO_REGION_SIZE;

void binload(const char *path)
{
    // Open the binary and copy it to memory
    int fd = open(path, O_RDONLY);
    if(fd == -1)
    {
        printf("Error: Couldnt open binary at %s\n", path);
        return;
    }
    struct stat fileStat;
    fstat(fd, &fileStat);
    if(fileStat.st_size + offset > MEMORY_SIZE)
    {
        printf("Error: Not enough memory available to load binary at %p\n", path);
        return;
    }
    ssize_t rbytes = read(fd, &mem[offset], fileStat.st_size);
    close(fd);

    // Get the header and patch it in memory
    sean816_rom_executable_header_t *header = (sean816_rom_executable_header_t*)&mem[offset];
    if(header->magic16 != SEAN816_HEADER_MAGIC) {
        printf("Error: Binary at path %s is not a valid Sean816 binary\n", path);
        return;
    }
    header->code_offset += offset;
    header->entry_offset += offset;
    offset += rbytes;

    // Relocate the offsets
    uint16_t *rloc_offsets = (uint16_t*)((uintptr_t)header + sizeof(sean816_rom_executable_header_t));
    for(uint16_t rloc = 0; rloc < header->reloc_count; rloc++)
        *(uint16_t*)(mem + header->code_offset + rloc_offsets[rloc]) += header->code_offset;

    cpu_core_t *core = cpu_create_core();
    core->pc = header->entry_offset;
    cpu_exec_core(core);
}
