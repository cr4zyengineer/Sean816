#include <stdlib.h>
#include <stdint.h>

#define SEAN816_HEADER_MAGIC 0xF27F

typedef struct {
    uint16_t magic16;
    uint16_t code_offset;
    uint16_t entry_offset;
    uint16_t reloc_count;
} sean816_rom_executable_header_t;
