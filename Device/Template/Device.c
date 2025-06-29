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
 * Header
 *
 */
#include <unistd.h>
#include <stdint.h>

/*
 * NOTE: Define this to get the offset, note: you have to nullterminate it!
 */
static uint16_t offsets[3] = {
    0x0001,
    0x0000
};

uint16_t* device_offset(void)
{
    return offsets;
}

/*
 *
 */
uint8_t some_device_value = 0;
void device_read(uint16_t addr, uint8_t *value)
{
    *value = some_device_value;
}

void device_write(uint16_t addr, uint8_t value)
{
    some_device_value = value;
}

/*
 * Initilizer
 */
void device_init(void)
{
    some_device_value = 27;
}
