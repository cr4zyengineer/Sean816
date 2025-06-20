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
