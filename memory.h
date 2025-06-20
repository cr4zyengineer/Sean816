/*
 * MIT License
 *
 * Copyright (c) 2025 Cr4zy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
    // IO Stuff
    bool isSet;
    void (*rfunction)(uint16_t addr, uint8_t *value);
    void (*wfunction)(uint16_t addr, uint8_t value);
} memory_io_mapping_t;

/*
 * Read and write
 *
 */
void memory_read(uint16_t addr, uint8_t *value);
void memory_write(uint16_t addr, uint8_t value);
