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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "code.h"
#include "../cpu.h"
#include "../ROM/header.h"

typedef struct {
    bool isLabel;
    char *modified_str;
    uint16_t offset;
} LABEL;

extern char *raw[MAX_LINES][MAX_WORDS];
extern int line_number;
static int raw_i = 0;
static uint8_t binary[UINT16_MAX];
static LABEL symbols[UINT16_MAX];
static uint16_t sym_count = 0;
static uint16_t roffset = 0;
static uint16_t reloc_count = 0;
static uint16_t reloc_offsets[UINT16_MAX];
static uint16_t main_symbol = UINT16_MAX;
static uint16_t cur_opcode_off = 0x0000;

bool is_number(const char *str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '\0') return false;

    while (*str) {
        if (!isdigit((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

LABEL labelcheck(const char *input) {
    LABEL result;
    size_t len = strlen(input);

    result.isLabel = false;
    result.modified_str = (char *)malloc(len + 1);

    if (input[len - 1] == ':') {
        strncpy(result.modified_str, input, len - 1);
        result.modified_str[len - 1] = '\0';
        result.isLabel = true;
    } else
        strcpy(result.modified_str, input);

    return result;
}

bool insertSymbolAddress(char *symbol_str)
{
    bool label_found = false;

    if (symbol_str[0] == '*') {
        char label[256] = {0};
        strcpy(label, symbol_str + 1);
        for (int h = 0; h < sym_count; h++)
            if (strcmp(label, symbols[h].modified_str) == 0) {
                reloc_offsets[reloc_count++] = roffset;
                binary[roffset++] = ((uint8_t *)&symbols[h].offset)[0];
                binary[roffset++] = ((uint8_t *)&symbols[h].offset)[1];
                label_found = true;
                break;
            }
    }

    return label_found;
}

int is_hex16_format(const char *str) {
    if((str == NULL || strlen(str) != 6) || (str[0] != '0' || str[1] != 'x'))
        return 0;

    for(int j = 2; j < 6; j++)
        if(!isxdigit((unsigned char)str[j]))
            return 0;

    return 1;
}

void enoughParam(char *s[6],
                 uint8_t minP,
                 uint8_t maxP)
{
    uint8_t param = 0;
    uint8_t raw_i_n = 1;

    while (raw_i_n < 6 && s[raw_i_n] != NULL && s[raw_i_n][0] != ';')
    {
        if(is_hex16_format(s[raw_i_n]) || (s[raw_i_n][0] == '*'))
            param++;
        param++;

        raw_i_n++;
    }

    if(param < minP || param > maxP)
    {
        if(minP != maxP)
            printf("Error:%d: Expected %d to %d parameters, but got %d\n", raw_i + 1, minP, maxP, param);
        else
            printf("Error:%d: Expected %d parameters, but got %d\n", raw_i + 1, minP, param);
        exit(1);
    }
}

bool opcmpintl(const char *a,
               char *b[6])
{
    return (strcmp(a, b[0]) == 0);
}

bool opcmp(const char *a,
           char *b[6],
           uint8_t c,
           uint8_t d,
           uint8_t opcode)
{
    bool isOpcode = opcmpintl(a, b);

    if(isOpcode)
    {
        enoughParam(b, c, d);
        if(opcode != 0xFF)
        {
            cur_opcode_off = roffset++;
            binary[cur_opcode_off] = opcode;
        } else {
            cur_opcode_off = 0xFFFF;
        }
    }

    return isOpcode;
}

int main(int argc,
         char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    read_file(argv[1]);

    for (int i = 0; i < line_number; i++) {
        // Patch for the helper instructions that take up more space
        if(raw[i][0] != NULL)

        for (int j = 0; j < MAX_WORDS; j++) {
            if (raw[i] == NULL || raw[i][j] == NULL || strcmp(raw[i][j], ";") == 0)
                break;

            if(is_hex16_format(raw[i][j]))
                roffset++;

            symbols[sym_count] = labelcheck(raw[i][j]);
            if(symbols[sym_count].isLabel) {
                if(main_symbol == 0xFFFF && strcmp(symbols[sym_count].modified_str, "main") == 0)
                    main_symbol = roffset;
                symbols[sym_count].offset = roffset;
                sym_count++;
                i++;

                // We need to recheck the patch when a label was found otherwise some labels will be misaligned!
                if(raw[i][0] != NULL)
                {
                    if(strcmp(raw[i][0], "str") == 0)
                        roffset += strlen(raw[i][1]) - 1;
                }
            } else if(raw[i][j][0] == '*')
                roffset++;
            roffset++;
        }
    }

    roffset = 0;
    for (raw_i = 0; raw_i < line_number; raw_i++) {
        if (raw[raw_i] == NULL || raw[raw_i][0] == NULL || raw[raw_i][0][0] == ';')
            continue;

        LABEL checklabel = labelcheck(raw[raw_i][0]);
        if(checklabel.isLabel) {
            raw_i++;
            if(raw[raw_i][0] == NULL) {
                raw_i++;
                continue;
            }
        }

        if(!(opcmp("halt", raw[raw_i], 0, 0, OP_HLT) ||
           opcmp("load", raw[raw_i], 3, 3, OP_LOAD) ||
           opcmp("store", raw[raw_i], 3, 3, OP_STORE) ||
           opcmp("mhml", raw[raw_i], 2, 2, OP_MHML) ||
           opcmp("loadlh", raw[raw_i], 1, 1, OP_LOADLH) ||
           opcmp("llh", raw[raw_i], 1, 1, OP_LOADLH) ||
           opcmp("storelh", raw[raw_i], 1, 1, OP_STORELH) ||
           opcmp("slh", raw[raw_i], 1, 1, OP_STORELH) ||
           opcmp("mov", raw[raw_i], 2, 2, OP_MOV) ||
           opcmp("add", raw[raw_i], 2, 2, OP_ADD) ||
           opcmp("sub", raw[raw_i], 2, 2, OP_SUB) ||
           opcmp("mul", raw[raw_i], 2, 2, OP_MUL) ||
           opcmp("div", raw[raw_i], 2, 2, OP_DIV) ||
           opcmp("inc", raw[raw_i], 1, 1, OP_INC) ||
           opcmp("dec", raw[raw_i], 1, 1, OP_DEC) ||
           opcmp("jmp", raw[raw_i], 2, 2, OP_JMP) ||
           opcmp("cmp", raw[raw_i], 2, 2, OP_CMP) ||
           opcmp("je", raw[raw_i], 2, 2, OP_JE) ||
           opcmp("jne", raw[raw_i], 2, 2, OP_JNE) ||
           opcmp("jg", raw[raw_i], 2, 2, OP_JG) ||
           opcmp("jl", raw[raw_i], 2, 2, OP_JL) ||
           opcmp("push", raw[raw_i], 1, 1, OP_PUSH) ||
           opcmp("pop", raw[raw_i], 1, 1, OP_POP) ||
           opcmp("call", raw[raw_i], 2, 2, OP_CALL) ||
           opcmp("calle", raw[raw_i], 2, 2, OP_CALLE) ||
           opcmp("callne", raw[raw_i], 2, 2, OP_CALLNE) ||
           opcmp("callg", raw[raw_i], 2, 2, OP_CALLG) ||
           opcmp("calll", raw[raw_i], 2, 2, OP_CALLL) ||
           opcmp("ret", raw[raw_i], 0, 0, OP_RET) ||
           opcmp("and", raw[raw_i], 2, 2, OP_AND) ||
           opcmp("or", raw[raw_i], 2, 2, OP_OR) ||
           opcmp("xor", raw[raw_i], 2, 2, OP_XOR) ||
           opcmp("not", raw[raw_i], 1, 1, OP_NOT) ||
           opcmp("str", raw[raw_i], 1, 1, 0xFF))) {
            printf("Error:%d: No such operation: %s\n", raw_i + 1, raw[raw_i][0]);
            return 1;
        }

        bool operandsig[3] = { false, false, false};
        uint8_t opi = 0;

        for (int j = 0; j < 3; j++) {
            if (raw[raw_i][j] == NULL)
                break;

            char *input = raw[raw_i][j + 1];

            if (input == NULL || input[0] == ';') break;

            operandsig[opi] = true;
            if(strcmp(input, "a") == 0)
                binary[roffset++] = 0x00;
            else if(strcmp(input, "b") == 0)
                binary[roffset++] = 0x01;
            else if(strcmp(input, "c") == 0)
                binary[roffset++] = 0x02;
            else if(strcmp(input, "d") == 0)
                binary[roffset++] = 0x03;
            else if(strcmp(input, "e") == 0)
                binary[roffset++] = 0x04;
            else if(strcmp(input, "f") == 0)
                binary[roffset++] = 0x05;
            else if(strcmp(input, "g") == 0)
                binary[roffset++] = 0x06;
            else if(strcmp(input, "h") == 0)
                binary[roffset++] = 0x07;
            else if(strcmp(input, "ra") == 0)
                binary[roffset++] = 0x08;
            else if(strcmp(input, "rb") == 0)
                binary[roffset++] = 0x09;
            else if(strcmp(input, "rc") == 0)
                binary[roffset++] = 0x0A;
            else if(strcmp(input, "rd") == 0)
                binary[roffset++] = 0x0B;
            else if(strcmp(input, "re") == 0)
                binary[roffset++] = 0x0C;
            else if(strcmp(input, "rf") == 0)
                binary[roffset++] = 0x0D;
            else if(strcmp(input, "rg") == 0)
                binary[roffset++] = 0x0E;
            else if(strcmp(input, "rh") == 0)
                binary[roffset++] = 0x0F;
            else if(strcmp(input, "ga") == 0)
                binary[roffset++] = 0x10;
            else if(strcmp(input, "gb") == 0)
                binary[roffset++] = 0x11;
            else if(strcmp(input, "gc") == 0)
                binary[roffset++] = 0x12;
            else if(strcmp(input, "gd") == 0)
                binary[roffset++] = 0x13;
            else if(strcmp(input, "ge") == 0)
                binary[roffset++] = 0x14;
            else if(strcmp(input, "gf") == 0)
                binary[roffset++] = 0x15;
            else if(strcmp(input, "gg") == 0)
                binary[roffset++] = 0x16;
            else if(strcmp(input, "gh") == 0)
                binary[roffset++] = 0x17;
            else if(strcmp(input, "ml") == 0)
                binary[roffset++] = 0x18;
            else if(strcmp(input, "mh") == 0)
                binary[roffset++] = 0x19;
            else if(strcmp(input, "cmp") == 0)
                binary[roffset++] = 0x1A;
            else if(strcmp(input, "spl") == 0)
                binary[roffset++] = 0x1B;
            else if(strcmp(input, "sph") == 0)
                binary[roffset++] = 0x1C;
            else if(strncmp(input, "0x", 2) == 0 || strncmp(input, "0X", 2) == 0) {
                size_t hex_digit_count = strlen(input + 2);
                unsigned long value = strtoul(input, NULL, 16);

                if(hex_digit_count == 2) {
                    binary[roffset++] = (uint8_t)value;
                    operandsig[opi] = false;
                } else if(hex_digit_count == 4) {
                    binary[roffset++] = (uint8_t)(value & 0xFF);
                    binary[roffset++] = (uint8_t)(value >> 8);
                    operandsig[opi++] = false;
                    operandsig[opi] = false;
                } else {
                    fprintf(stderr, "Error:%d: Invalid hex format (too large) for %s\n", raw_i + 1, input);
                    return 1;
                }
            } else if(is_number(input)) {
                binary[roffset++] = (uint8_t)(atoi(input));
                operandsig[opi] = false;
            } else if(strcmp("str", raw[raw_i][0]) == 0) {
                size_t len = strlen(input);
                for(size_t i = 0; i < len; i++)
                    binary[roffset++] = input[i];
                binary[roffset++] = '\0';
            } else if(!insertSymbolAddress(input)) {
                printf("Error:%d: Unknown parameter type for %s\n", raw_i + 1, input);
                return 1;
            } else {
                operandsig[opi++] = false;
                operandsig[opi] = false;
            }

            opi++;
        }

        if(cur_opcode_off != 0xFFFF)
        {
            binary[cur_opcode_off] |= (operandsig[0] << 5);
            binary[cur_opcode_off] |= (operandsig[1] << 6);
            binary[cur_opcode_off] |= (operandsig[2] << 7);
        }
    }

    // Freeing raw content
    free_content();

    // Checking for main symbol
    if(main_symbol == 0xFFFF)
    {
        printf("Error: \"main\" symbol not found!\n");
        return 1;
    }

    // Preparing final binary creation
    uint16_t final_offset = sizeof(sean816_rom_executable_header_t);
    uint8_t final_binary[UINT16_MAX];

    // Copy reloc table section to final binary
    memcpy((void*)(final_binary + final_offset), (uint8_t*)&reloc_offsets, sizeof(uint16_t) * reloc_count);
    final_offset += (sizeof(uint16_t) * reloc_count);

    // Copy code section to final binary
    size_t binary_start_offset = final_offset;
    memcpy((void*)(final_binary + final_offset), binary, roffset);
    final_offset += roffset;

    // Generating header into final binaries header section
    sean816_rom_executable_header_t *header = (sean816_rom_executable_header_t*)&final_binary[0x00];
    header->magic16 = SEAN816_HEADER_MAGIC;
    header->code_offset = binary_start_offset;
    header->entry_offset = binary_start_offset + main_symbol;
    header->reloc_count = reloc_count;

    // Writing the final binary to the user specified location
    remove(argv[2]);
    int fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0755);
    if(fd == -1)
    {
        printf("Error: Failed to open binary file\n");
        return 1;
    }
    write(fd, final_binary, final_offset);

    return 0;
}
