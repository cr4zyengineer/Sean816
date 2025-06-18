//
// main.c
// Sean16Compiler
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "code.h"
#include "../cpu.h"
#include "../ROM/header.h"

typedef struct {
    char *modified_str;
    bool had_colon;
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

    result.had_colon = false;
    result.modified_str = (char *)malloc(len + 1);

    if (input[len - 1] == ':') {
        strncpy(result.modified_str, input, len - 1);
        result.modified_str[len - 1] = '\0';
        result.had_colon = true;
    } else {
        strcpy(result.modified_str, input);
    }

    return result;
}

bool islabel(const char *input) {
    return (input[0] == '*');
}

void insertSymbolAddress(char *symbol_str)
{
    bool label_found = false;

    if (symbol_str[0] == '*') {
        char label[256] = {0};
        strcpy(label, symbol_str + 1);
        for (int h = 0; h < sym_count; h++) {
            if (symbols[h].had_colon && strcmp(label, symbols[h].modified_str) == 0) {
                reloc_offsets[reloc_count++] = roffset;
                binary[roffset++] = ((uint8_t *)&symbols[h].offset)[0];
                binary[roffset++] = ((uint8_t *)&symbols[h].offset)[1];
                label_found = true;
                break;
            }
        }
    }

    if (!label_found) {
        printf("Error:%d: Unknown label \"%s\"\n", raw_i + 1, symbol_str + 1);
        exit(1);
    }
}

int is_hex16_format(const char *str) {
    if ((str == NULL || strlen(str) != 6) || (str[0] != '0' || str[1] != 'x'))
        return 0;

    for (int j = 2; j < 6; j++) {
        if (!isxdigit((unsigned char)str[j]))
            return 0;
    }

    return 1;
}

void enoughParam(char *s[6], uint8_t minP, uint8_t maxP)
{
    uint8_t param = 0;
    uint8_t raw_i_n = 1;

    while (raw_i_n < 6 && s[raw_i_n] != NULL && s[raw_i_n][0] != ';')
    {
        if (is_hex16_format(s[raw_i_n]) || s[raw_i_n][0] == '*' || s[raw_i_n][0] == '"')
            param += 2;
        else
            param++;

        raw_i_n++;
    }

    if (param < minP || param > maxP)
    {
        printf("Error:%d: Expected %d to %d parameters, but got %d\n", raw_i + 1, minP, maxP, param);
        exit(1);
    }
}

bool opcmp(const char *a, char *b[6], uint8_t c, uint8_t d)
{
    bool isOpcode = (strcmp(a, b[0]) == 0);

    if(isOpcode)
        enoughParam(b, c, d);

    return isOpcode;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    read_file(argv[1]);

    for (int i = 0; i < line_number; i++) {
        // Patch for the helper instructions that take up more space
        if(raw[i][0] != NULL)
            if(strcmp(raw[i][0], "limm") == 0 || strcmp(raw[i][0], "lmem") == 0)
                roffset++;

        for (int j = 0; j < MAX_WORDS; j++) {
            if (raw[i] == NULL || raw[i][j] == NULL || strcmp(raw[i][j], ";") == 0)
                break;

            if(is_hex16_format(raw[i][j]))
                roffset++;

            symbols[sym_count] = labelcheck(raw[i][j]);
            if(symbols[sym_count].had_colon) {
                if(main_symbol == 0xFFFF && strcmp(symbols[sym_count].modified_str, "main") == 0)
                    main_symbol = roffset;
                symbols[sym_count].offset = roffset;
                sym_count++;
                i++;

                // We need to recheck the patch when a label was found otherwise some labels will be misaligned!
                if(raw[i][0] != NULL)
                    if(strcmp(raw[i][0], "limm") == 0 || strcmp(raw[i][0], "lmem") == 0)
                        roffset++;

            } else if(islabel(raw[i][j]))
                roffset++;
            roffset++;
        }
    }

    roffset = 0;
    for (raw_i = 0; raw_i < line_number; raw_i++) {
        if (raw[raw_i] == NULL || raw[raw_i][0] == NULL || raw[raw_i][0][0] == ';')
            continue;

        LABEL checklabel = labelcheck(raw[raw_i][0]);
        if(checklabel.had_colon) {
            raw_i++;
            if(raw[raw_i][0] == NULL) {
                raw_i++;
                continue;
            }
        }

        if(opcmp("halt", raw[raw_i], 0, 0))
            binary[roffset++] = OP_HLT;
        else if(opcmp("load", raw[raw_i], 2, 3))
            binary[roffset++] = OP_LOAD;
        else if(opcmp("limm", raw[raw_i], 2, 2)) {
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x00;
        } else if(opcmp("lmem", raw[raw_i], 3, 3)) {
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x01;
        } else if(opcmp("store", raw[raw_i], 3, 3))
            binary[roffset++] = OP_STORE;
        else if(opcmp("mhml", raw[raw_i], 2, 2))
            binary[roffset++] = OP_MHML;
        else if(opcmp("loadlh", raw[raw_i], 1, 1) || opcmp("llh", raw[raw_i], 1, 1))
            binary[roffset++] = OP_LOADLH;
        else if(opcmp("storelh", raw[raw_i], 1, 1) || opcmp("slh", raw[raw_i], 1, 1))
            binary[roffset++] = OP_STORELH;
        else if(opcmp("mov", raw[raw_i], 2, 2))
            binary[roffset++] = OP_MOV;
        else if(opcmp("add", raw[raw_i], 2, 2))
            binary[roffset++] = OP_ADD;
        else if(opcmp("sub", raw[raw_i], 2, 2))
            binary[roffset++] = OP_SUB;
        else if(opcmp("mul", raw[raw_i], 2, 2))
            binary[roffset++] = OP_MUL;
        else if(opcmp("div", raw[raw_i], 2, 2))
            binary[roffset++] = OP_DIV;
        else if(opcmp("inc", raw[raw_i], 1, 1))
            binary[roffset++] = OP_INC;
        else if(opcmp("dec", raw[raw_i], 1, 1))
            binary[roffset++] = OP_DEC;
        else if(opcmp("jmp", raw[raw_i], 2, 2))
            binary[roffset++] = OP_JMP;
        else if(opcmp("cmp", raw[raw_i], 2, 2))
            binary[roffset++] = OP_CMP;
        else if(opcmp("je", raw[raw_i], 2, 2))
            binary[roffset++] = OP_JE;
        else if(opcmp("jne", raw[raw_i], 2, 2))
            binary[roffset++] = OP_JNE;
        else if(opcmp("jg", raw[raw_i], 2, 2))
            binary[roffset++] = OP_JG;
        else if(opcmp("jl", raw[raw_i], 2, 2))
            binary[roffset++] = OP_JL;
        else if(opcmp("push", raw[raw_i], 1, 1))
            binary[roffset++] = OP_PUSH;
        else if(opcmp("pop", raw[raw_i], 1, 1))
            binary[roffset++] = OP_POP;
        else if(opcmp("call", raw[raw_i], 2, 2))
            binary[roffset++] = OP_CALL;
        else if(opcmp("ret", raw[raw_i], 0, 0))
            binary[roffset++] = OP_RET;
        else if(opcmp("and", raw[raw_i], 2, 2))
            binary[roffset++] = OP_AND;
        else if(opcmp("or", raw[raw_i], 2, 2))
            binary[roffset++] = OP_OR;
        else if(opcmp("xor", raw[raw_i], 2, 2))
            binary[roffset++] = OP_XOR;
        else if(opcmp("not", raw[raw_i], 1, 1))
            binary[roffset++] = OP_NOT;
        else {
            printf("Error:%d: No such operation: %s\n", raw_i + 1, raw[raw_i][0]);
            return 1;
        }

        for (int j = 0; j < MAX_WORDS; j++) {
            if (raw[raw_i][j] == NULL)
                break;

            char *input = raw[raw_i][j + 1];

            if (input == NULL || input[0] == ';') break;

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
            else if(strcmp(input, "mo") == 0)
                binary[roffset++] = 0x18;
            else if(strcmp(input, "ml") == 0)
                binary[roffset++] = 0x19;
            else if(strcmp(input, "mh") == 0)
                binary[roffset++] = 0x1A;
            else if(strcmp(input, "spl") == 0)
                binary[roffset++] = 0x1B;
            else if(strcmp(input, "sph") == 0)
                binary[roffset++] = 0x1C;
            else if(input[0] == '*')
                insertSymbolAddress(input);
            else if(strncmp(input, "0x", 2) == 0 || strncmp(input, "0X", 2) == 0) {
                size_t hex_digit_count = strlen(input + 2);
                unsigned long value = strtoul(input, NULL, 16);

                if(hex_digit_count == 2)
                    binary[roffset++] = (uint8_t)value;
                else if(hex_digit_count == 4) {
                    binary[roffset++] = (uint8_t)(value & 0xFF);   // Low byte
                    binary[roffset++] = (uint8_t)(value >> 8);     // High byte
                } else {
                    fprintf(stderr, "Error:%d: Invalid hex format (too large) for %s\n", raw_i + 1, input);
                    return 1;
                }
            } else if(is_number(input)) {
                binary[roffset++] = (uint8_t)(atoi(input));
            } else {
                printf("Error:%d: Unknown parameter type for %s\n", raw_i + 1, input);
                return 1;
            }
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
    int fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0755);
    if(fd == -1)
    {
        printf("Error: Failed to open binary file\n");
        return 1;
    }
    write(fd, final_binary, final_offset);

    return 0;
}
