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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "code.h"

char *raw[MAX_LINES][MAX_WORDS];
int line_number = 0;

void read_file(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && line_number < MAX_LINES) {
        int word_number = 0;
        char *p = line;

        while (*p && word_number < MAX_WORDS) {
            // Skip leading whitespace
            while (isspace(*p)) p++;

            if (*p == '\0' || *p == '\n') break;

            char *start;
            char word[MAX_LINE_LENGTH];  // temporary buffer for a word
            size_t len = 0;

            if (*p == '"') {
                p++;
                while (*p && *p != '"') {
                    if (*p == '\\') {
                        p++;
                        switch (*p) {
                        case 'n': word[len++] = '\n'; break;
                        case 't': word[len++] = '\t'; break;
                        case 'r': word[len++] = '\r'; break;
                        case '\\': word[len++] = '\\'; break;
                        case '"': word[len++] = '"'; break;
                        case '0': word[len++] = '\0'; break;
                        default:
                            word[len++] = '\\';
                            if (*p) word[len++] = *p;
                            break;
                        }
                        if (*p) p++;
                    } else {
                        word[len++] = *p++;
                    }
                    if (len >= MAX_LINE_LENGTH - 1) break;
                }
                if (*p == '"') p++;
            } else {
                start = p;
                while (*p && !isspace(*p)) {
                    if (len < MAX_LINE_LENGTH - 1) word[len++] = *p;
                    p++;
                }
            }

            word[len] = '\0';

            // Allocate and store the word
            raw[line_number][word_number] = malloc(len + 1);
            if (raw[line_number][word_number] == NULL) {
                printf("Memory allocation failed\n");
                exit(1);
            }
            strcpy(raw[line_number][word_number], word);
            word_number++;
        }

        // Null out remaining entries
        memset(&raw[line_number][word_number], 0, (MAX_WORDS - word_number) * sizeof(char *));
        line_number++;
    }

    fclose(file);
}

void free_content() {
    for (int i = 0; i < MAX_LINES; i++) {
        for (int j = 0; j < MAX_WORDS; j++) {
            if (raw[i][j] != NULL) {
                free(raw[i][j]);
            }
        }
    }
}
