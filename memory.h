/*
 * Headers
 *
 */
#include <stdint.h>
#include <stdbool.h>

/*
 * Definitions
 *
 */
#define MEMORY_SIZE 0x6000
#define MEMORY_MAPPED_IO_REGION_SIZE 0x00FF

typedef struct {
    // RAW Memory Stuff
    uint8_t value;

    // IO Stuff
    bool isSet;
    void (*rfunction)(uint16_t addr, uint8_t *value);
    void (*wfunction)(uint16_t addr, uint8_t value);
} memory_io_mapping_t;

/*
 * IO Memory shit
 *
 */
void memory_io_set(uint16_t addr, void *rfunction, void *wfunction);

/*
 * Read and write
 *
 */
void memory_read(uint16_t addr, uint8_t *value);
void memory_write(uint16_t addr, uint8_t value);
void memory_buffer_intl_exec(uint8_t buffer[UINT16_MAX - MEMORY_MAPPED_IO_REGION_SIZE]);
void memory_load_binary(const char *filePath);
