#include <stdlib.h>
#include <stdint.h>

#define SEAN816_HEADER_MAGIC 27

typedef struct {
    uint8_t magic;
    uint16_t entry;
    uint16_t reloc_count; // Relocations are exactly after the header
} sean816_rom_executable_header_t;
