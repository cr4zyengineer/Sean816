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
#include "libasmfile.h"
#include "code.h"
#include "../cpu.h"
#include "../ROM/header.h"

typedef struct {
    char *modified_str;
    bool had_colon;
    uint16_t offset;
} LABEL;

static char *(*raw)[MAX_WORDS];
static int raw_i = 0;
static uint8_t data[UINT16_MAX];
static uint8_t binary[UINT16_MAX];
static LABEL symbols[UINT16_MAX];
static uint16_t sym_count = 0;
static uint16_t doffset = 0;
static uint16_t roffset = 0;
static uint16_t reloc_count = 0;
static uint16_t reloc_offsets[UINT16_MAX];
static uint16_t data_count = 0;
static uint16_t data_reloc_offsets[UINT16_MAX];

bool is_number(const char *str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '-' || *str == '+') str++;
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
                sprintf(raw[raw_i][1], "H%d", symbols[h].offset);
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
    // Check length: must be exactly 6 characters ("0x" + 4 hex digits)
    if (str == NULL || strlen(str) != 6)
        return 0;
    // Check prefix "0x"
    if (str[0] != '0' || str[1] != 'x')
        return 0;
    // Check that the next 4 characters are hex digits
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

uint16_t addData(const char *str)
{
    uint16_t offset = doffset;
    data_reloc_offsets[data_count++] = roffset;
    for(size_t i = 1; str[i] != '"'; i++)
        data[doffset++] = str[i];
    data[doffset++] = '\0';
    return offset;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    printf("[*] Sean816 Assembler\n");

    raw = read_file(argv[1]);

    printf("[*] Getting label addresses\n");
    for (int i = 0; i < MAX_LINES; i++) {

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
                symbols[sym_count].offset = roffset;
                sym_count++;
                i++;

                // We need to recheck the patch when a label was found otherwise some labels will be misaligned!
                if(raw[i][0] != NULL)
                    if(strcmp(raw[i][0], "limm") == 0 || strcmp(raw[i][0], "lmem") == 0)
                        roffset++;

            } else if(islabel(raw[i][j])) {
                roffset++;
            }

            roffset++;
        }
    }

    roffset = 0;
    printf("[*] compile\n");
    for (raw_i = 0; raw_i < MAX_LINES; raw_i++) {
        if (raw[raw_i] == NULL || raw[raw_i][0] == NULL || raw[raw_i][0][0] == ';') {
            continue;
        }

        LABEL checklabel = labelcheck(raw[raw_i][0]);
        if(checklabel.had_colon) {
            raw_i++;
        }

        if (strcmp("halt", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 0, 0);
            binary[roffset++] = OP_HLT;
        } else if (strcmp("load", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 3);
            binary[roffset++] = OP_LOAD;
        } else if (strcmp("limm", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x00;
        } else if (strcmp("lmem", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 3, 3);
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x01;
        } else if (strcmp("store", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 3, 3);
            binary[roffset++] = OP_STORE;
        } else if (strcmp("mhml", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_MHML;
        } else if (strcmp("loadlh", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_LOADLH;
        } else if (strcmp("storelh", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_STORELH;
        } else if (strcmp("llh", raw[raw_i][0]) == 0) {         // Alternative names to shorten the instruction name
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_LOADLH;
        } else if (strcmp("slh", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_STORELH;
        } else if (strcmp("mov", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_MOV;
        } else if (strcmp("add", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_ADD;
        } else if (strcmp("sub", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_SUB;
        } else if (strcmp("mul", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_MUL;
        } else if (strcmp("div", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_DIV;
        } else if (strcmp("inc", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_INC;
        } else if (strcmp("dec", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_DEC;
        } else if (strcmp("jmp", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JMP;
        } else if (strcmp("cmp", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_CMP;
        } else if (strcmp("je", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JE;
        } else if (strcmp("jne", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JNE;
        } else if (strcmp("jg", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JG;
        } else if (strcmp("jl", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JL;
        } else if (strcmp("push", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_PUSH;
        } else if (strcmp("pop", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_POP;
        } else if (strcmp("call", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_CALL;
        } else if (strcmp("ret", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 0, 0);
            binary[roffset++] = OP_RET;
        } else if (strcmp("and", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_AND;
        } else if (strcmp("or", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_OR;
        } else if (strcmp("xor", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_XOR;
        } else if (strcmp("not", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_NOT;
        } else {
            printf("Error:%d: No such operation: %s\n", raw_i + 1, raw[raw_i][0]);
            return 1;
        }

        for (int j = 0; j < 5; j++) {
            if (raw[raw_i][j + 1] == NULL)
                break;

            char *input = raw[raw_i][j + 1];

            if (input == NULL) break;
            if (input[0] == ';') break;

            if(strcmp(input, "a") == 0) {
                binary[roffset++] = 0x00;
            } else if(strcmp(input, "b") == 0) {
                binary[roffset++] = 0x01;
            } else if(strcmp(input, "c") == 0) {
                binary[roffset++] = 0x02;
            } else if(strcmp(input, "d") == 0) {
                binary[roffset++] = 0x03;
            } else if(strcmp(input, "e") == 0) {
                binary[roffset++] = 0x04;
            } else if(strcmp(input, "f") == 0) {
                binary[roffset++] = 0x05;
            } else if(strcmp(input, "g") == 0) {
                binary[roffset++] = 0x06;
            } else if(strcmp(input, "h") == 0) {
                binary[roffset++] = 0x07;
            } else if(strcmp(input, "ra") == 0) {
                binary[roffset++] = 0x08;
            } else if(strcmp(input, "rb") == 0) {
                binary[roffset++] = 0x09;
            } else if(strcmp(input, "rc") == 0) {
                binary[roffset++] = 0x0A;
            } else if(strcmp(input, "rd") == 0) {
                binary[roffset++] = 0x0B;
            } else if(strcmp(input, "re") == 0) {
                binary[roffset++] = 0x0C;
            } else if(strcmp(input, "rf") == 0) {
                binary[roffset++] = 0x0D;
            } else if(strcmp(input, "rg") == 0) {
                binary[roffset++] = 0x0E;
            } else if(strcmp(input, "rh") == 0) {
                binary[roffset++] = 0x0F;
            } else if(strcmp(input, "ga") == 0) {
                binary[roffset++] = 0x10;
            } else if(strcmp(input, "gb") == 0) {
                binary[roffset++] = 0x11;
            } else if(strcmp(input, "gc") == 0) {
                binary[roffset++] = 0x12;
            } else if(strcmp(input, "gd") == 0) {
                binary[roffset++] = 0x13;
            } else if(strcmp(input, "ge") == 0) {
                binary[roffset++] = 0x14;
            } else if(strcmp(input, "gf") == 0) {
                binary[roffset++] = 0x15;
            } else if(strcmp(input, "gg") == 0) {
                binary[roffset++] = 0x16;
            } else if(strcmp(input, "gh") == 0) {
                binary[roffset++] = 0x17;
            } else if(strcmp(input, "mo") == 0) {
                binary[roffset++] = 0x18;
            } else if(strcmp(input, "ml") == 0) {
                binary[roffset++] = 0x19;
            } else if(strcmp(input, "mh") == 0) {
                binary[roffset++] = 0x1A;
            } else if(strcmp(input, "spl") == 0) {
                binary[roffset++] = 0x1B;
            } else if(strcmp(input, "sph") == 0) {
                binary[roffset++] = 0x1C;
            } else if(input[0] == '"') {
                uint16_t offset = addData(input);
                binary[roffset++] = ((uint8_t*)&offset)[0];
                binary[roffset++] = ((uint8_t*)&offset)[1];
            } else if(input[0] == '*') {
                insertSymbolAddress(input);
                input++;
                uint16_t value = (uint16_t)atoi(input);
                uint8_t *array = (uint8_t *)&value;
                binary[roffset++] = array[0];
                binary[roffset++] = array[1];
            } else if (input[0] == 'H') {
                input++; // Skip 'H'
                uint16_t value = (uint16_t)atoi(input);
                uint8_t *array = (uint8_t *)&value;
                binary[roffset++] = array[0];
                binary[roffset++] = array[1];
            } else if (strncmp(input, "0x", 2) == 0 || strncmp(input, "0X", 2) == 0) {
                size_t hex_digit_count = strlen(input + 2); // how many digits after 0x
                unsigned long value = strtoul(input, NULL, 16); // convert hex string

                if (hex_digit_count <= 2) {
                    // Treat as 8-bit
                    binary[roffset++] = (uint8_t)value;
                } else if (hex_digit_count <= 4) {
                    // Treat as 16-bit
                    binary[roffset++] = (uint8_t)(value & 0xFF);   // Low byte
                    binary[roffset++] = (uint8_t)(value >> 8);     // High byte
                } else {
                    fprintf(stderr, "Invalid hex format (too large): %s\n", input);
                }
            } else if(is_number(input)) {
                int number = atoi(input);
                binary[roffset++] = (uint8_t)(number);
            } else {
                printf("Error:%d: Unknown parameter type for %s\n", raw_i + 1, input);
                return 1;
            }
        }
    }

    // Generating final binary
    uint16_t final_offset = sizeof(sean816_rom_executable_header_t);
    uint16_t old_offset = 0;
    uint8_t final_binary[UINT16_MAX];

    bool label_found = false;
    uint16_t symbol = 0x00;
    for (int h = 0; h < sym_count; h++) {
        if (symbols[h].had_colon && strcmp("main", symbols[h].modified_str) == 0) {
            symbol = symbols[h].offset;
            label_found = true;
            break;
        }
    }

    if(!label_found)
    {
        printf("Error:Internal: \"main\" Symbol not found!\n");
        return 1;
    }

    // NOTE: Here we prepare buffers before the copy over
    uint8_t *reloc_buffer = (uint8_t*)&reloc_offsets;
    uint8_t *data_buffer = data;

    // NOTE: Copy reloc offset table
    for(old_offset = final_offset; final_offset < ((sizeof(uint16_t) * reloc_count) + old_offset); final_offset++)
        final_binary[final_offset] = reloc_buffer[final_offset - old_offset];

    // NOTE: Copy data section
    for(old_offset = final_offset; final_offset < doffset + old_offset; final_offset++)
        final_binary[final_offset] = data_buffer[final_offset - old_offset];

    printf("[*] data section is %d bytes long\n", final_offset - old_offset);

    uint16_t binary_start_offset = final_offset;
    memcpy((void*)(final_binary + binary_start_offset), binary, roffset);

    sean816_rom_executable_header_t *header = (sean816_rom_executable_header_t*)&final_binary[0x00];
    header->magic = SEAN816_HEADER_MAGIC;
    header->code_offset = binary_start_offset;
    header->entry_offset = binary_start_offset + symbol;
    header->reloc_count = reloc_count;

    storeasm816(argv[2], final_binary, final_offset + roffset);
    free_content(raw);

    printf("[*] binary size: %db\n", final_offset + roffset);

    printf("[*] done :3\n");

    return 0;
}
