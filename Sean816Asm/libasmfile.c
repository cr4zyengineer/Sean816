//
// libasmfile.c
// Sean16Compiler
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_INSTRUCTIONS 1000
#define MAX_ARGS 6

/*
 * Sean816
 */
void storeasm816(const char *filename, uint8_t *values, size_t size) {
    // Open the file in binary write mode
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write the array of 8-bit values to the file
    size_t written = fwrite(values, sizeof(uint8_t), size, file);
    if (written != size) {
        perror("Error writing to file");
    }

    // Close the file
    fclose(file);
}
