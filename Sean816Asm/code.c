#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        char *word = strtok(line, " \t\n");

        while (word != NULL && word_number < MAX_WORDS) {
            size_t word_size = strlen(word);
            raw[line_number][word_number] = malloc(word_size + 1);
            if (raw[line_number][word_number] == NULL) {
                printf("Memory allocation failed\n");
                exit(1);
            }
            memcpy(raw[line_number][word_number], word, word_size);
            raw[line_number][word_number][word_size] = '\0';

            word_number++;
            word = strtok(NULL, " \t\n");
        }

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
