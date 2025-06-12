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
static uint8_t binary[UINT16_MAX];
static LABEL symbols[UINT16_MAX];
static uint16_t sym_count = 0;
static uint16_t roffset = 0;
static uint16_t reloc_count = 0;
static uint16_t reloc_offsets[UINT16_MAX];

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
        reloc_offsets[reloc_count++] = roffset;
        sprintf(symbol_str, "H%u", roffset + atoi(symbol_str));
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
        if (is_hex16_format(s[raw_i_n]) || s[raw_i_n][0] == '*')
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
            if(strcmp(raw[i][0], "LIMM") == 0 || strcmp(raw[i][0], "LMEM") == 0)
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
                    if(strcmp(raw[i][0], "LIMM") == 0 || strcmp(raw[i][0], "LMEM") == 0)
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
        if (raw[raw_i] == NULL || raw[raw_i][0] == NULL) {
            continue;
        }

        LABEL checklabel = labelcheck(raw[raw_i][0]);
        if(checklabel.had_colon) {
            raw_i++;
        }

        if (strcmp("HALT", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 0, 0);
            binary[roffset++] = OP_HLT;
        } else if (strcmp("LOAD", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 3);
            binary[roffset++] = OP_LOAD;
        } else if (strcmp("LIMM", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x00;
        } else if (strcmp("LMEM", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 3, 3);
            binary[roffset++] = OP_LOAD;
            binary[roffset++] = 0x01;
        } else if (strcmp("STORE", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 3, 3);
            binary[roffset++] = OP_STORE;
        } else if (strcmp("LOADLH", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_LOADLH;
        } else if (strcmp("STORELH", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_STORELH;
        } else if (strcmp("LLH", raw[raw_i][0]) == 0) {         // Alternative names to shorten the instruction name
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_LOADLH;
        } else if (strcmp("SLH", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_STORELH;
        } else if (strcmp("MOV", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_MOV;
        } else if (strcmp("ADD", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_ADD;
        } else if (strcmp("SUB", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_SUB;
        } else if (strcmp("MUL", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_MUL;
        } else if (strcmp("DIV", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_DIV;
        } else if (strcmp("INC", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_INC;
        } else if (strcmp("DEC", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_DEC;
        } else if (strcmp("JMP", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JMP;
        } else if (strcmp("CMP", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_CMP;
        } else if (strcmp("JE", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JE;
        } else if (strcmp("JNE", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JNE;
        } else if (strcmp("JG", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JG;
        } else if (strcmp("JL", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_JL;
        } else if (strcmp("PUSH", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_PUSH;
        } else if (strcmp("POP", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 1, 1);
            binary[roffset++] = OP_POP;
        } else if (strcmp("CALL", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 2, 2);
            binary[roffset++] = OP_CALL;
        } else if (strcmp("RET", raw[raw_i][0]) == 0) {
            enoughParam(raw[raw_i], 0, 0);
            binary[roffset++] = OP_RET;
        } else {
            printf("Error:%d: No such operation: %s\n", raw_i + 1, raw[raw_i][0]);
            return 1;
        }

        char *input = NULL;
        for (int j = 0; j < 5; j++) {
            if (raw[raw_i][j + 1] != NULL) {
                input = raw[raw_i][j + 1];

                if (input == NULL) break;
                if (input[0] == ';') break;

                if(input[0] == '*') {
                    insertSymbolAddress(input);
                    input++;
                    uint16_t value = (uint16_t)atoi(input);
                    uint8_t *array = (uint8_t *)&value;
                    binary[roffset++] = array[1];
                    binary[roffset++] = array[0];
                }
                else if(input[0] == '%') {
                    switch(input[1]) {
                        case 'A':
                            binary[roffset++] = 0x00;
                            break;
                        case 'B':
                            binary[roffset++] = 0x01;
                            break;
                        case 'C':
                            binary[roffset++] = 0x02;
                            break;
                        case 'D':
                            binary[roffset++] = 0x03;
                            break;
                        case 'E':
                            binary[roffset++] = 0x04;
                            break;
                        case 'F':
                            binary[roffset++] = 0x05;
                            break;
                        case 'H':
                            binary[roffset++] = 0x07;
                            break;
                        case 'R':
                            switch(input[2])
                            {
                                case 'A':
                                    binary[roffset++] = 0x08;
                                    break;
                                case 'B':
                                    binary[roffset++] = 0x09;
                                    break;
                                case 'C':
                                    binary[roffset++] = 0x0A;
                                    break;
                                case 'D':
                                    binary[roffset++] = 0x0B;
                                    break;
                                case 'E':
                                    binary[roffset++] = 0x0C;
                                    break;
                                case 'F':
                                    binary[roffset++] = 0x0D;
                                    break;
                                case 'G':
                                    binary[roffset++] = 0x0E;
                                    break;
                                case 'H':
                                    binary[roffset++] = 0x0F;
                                    break;
                                default:
                                    binary[roffset++] = 0x00;
                            }
                            break;
                        case 'G':
                            switch(input[2])
                            {
                                case 'A':
                                    binary[roffset++] = 0x10;
                                    break;
                                case 'B':
                                    binary[roffset++] = 0x11;
                                    break;
                                case 'C':
                                    binary[roffset++] = 0x12;
                                    break;
                                case 'D':
                                    binary[roffset++] = 0x13;
                                    break;
                                case 'E':
                                    binary[roffset++] = 0x14;
                                    break;
                                case 'F':
                                    binary[roffset++] = 0x15;
                                    break;
                                case 'G':
                                    binary[roffset++] = 0x16;
                                    break;
                                case 'H':
                                    binary[roffset++] = 0x17;
                                    break;
                                default:
                                    binary[roffset++] = 0x06;
                            }
                            break;
                        case 'M':
                            switch(input[2])
                            {
                                case 'O':
                                    binary[roffset++] = 0x18;
                                    break;
                                case 'L':
                                    binary[roffset++] = 0x19;
                                    break;
                                case 'H':
                                    binary[roffset++] = 0x1A;
                                    break;
                                default:
                                    binary[roffset++] = 0x00;
                            }
                            break;
                        default:
                            binary[roffset++] = 0x00;
                            break;
                    }
                }
                else if (input[0] == 'H') {
                    input++; // Skip 'H'
                    uint16_t value = (uint16_t)atoi(input);
                    uint8_t *array = (uint8_t *)&value;
                    binary[roffset++] = array[1];
                    binary[roffset++] = array[0];
                }
                else if (strncmp(input, "0x", 2) == 0 || strncmp(input, "0X", 2) == 0) {
                    size_t hex_digit_count = strlen(input + 2); // how many digits after 0x
                    unsigned long value = strtoul(input, NULL, 16); // convert hex string

                    if (hex_digit_count <= 2) {
                        // Treat as 8-bit
                        binary[roffset++] = (uint8_t)value;
                    } else if (hex_digit_count <= 4) {
                        // Treat as 16-bit
                        binary[roffset++] = (uint8_t)(value >> 8);     // High byte
                        binary[roffset++] = (uint8_t)(value & 0xFF);   // Low byte
                    } else {
                        fprintf(stderr, "Invalid hex format (too large): %s\n", input);
                    }
                }
                else {
                    int number = atoi(input);
                    binary[roffset++] = (uint8_t)(number);
                }
            } else {
                //printf("Error:%d: Unknown parameter type for %s\n", raw_i + 1, input);
                //return 1;
                break;
            }
        }
    }

    sean816_rom_executable_header_t *header = malloc(sizeof(sean816_rom_executable_header_t));
    header->magic = SEAN816_HEADER_MAGIC;
    header->entry = 0x00;
    header->reloc_count = reloc_count;

    // NOTE: Here we prepare buffers before the copy over
    uint8_t *header_buffer = (uint8_t*)header;
    uint8_t *reloc_buffer = (uint8_t*)&reloc_offsets;

    // Generating final binary
    uint16_t final_offset = 0;
    uint16_t old_offset = 0;
    uint8_t final_binary[UINT16_MAX];

    for(final_offset = 0; final_offset < sizeof(sean816_rom_executable_header_t); final_offset++)
        final_binary[final_offset] = header_buffer[final_offset];

    for(old_offset = final_offset; final_offset < ((sizeof(uint16_t) * reloc_count) + old_offset); final_offset++)
        final_binary[final_offset] = reloc_buffer[final_offset - old_offset];

    uint16_t binary_start_offset = final_offset;
    memcpy((void*)(final_binary + binary_start_offset), binary, roffset);

    storeasm816(argv[2], final_binary, final_offset + roffset);
    free_content(raw);

    printf("[*] binary size: %db\n", final_offset + roffset);

    printf("[*] done :3\n");

    return 0;
}
