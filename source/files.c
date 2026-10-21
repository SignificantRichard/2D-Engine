#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>

uint8_t* readBinFromFile(const char* fileName, size_t* outNumInts) {
    struct stat info;
    
    if (stat(fileName, &info) != 0) {
        perror("Failed to read file stats");
        return NULL;
    }

    FILE* fptr = fopen(fileName, "rb");
    if (fptr == NULL) {
        perror("Failed to open file");
        return NULL;
    }
    
    char* content = malloc(info.st_size);
    if (content == NULL) {
        perror("Failed to allocate memory for file content");
        fclose(fptr);
        return NULL;
    }

    size_t bytesRead = fread(content, 1, info.st_size, fptr);
    fclose(fptr);

    if (bytesRead < sizeof(uint8_t)) {
        fprintf(stderr, "Error: File is too small to contain 32-bit integers.\n");
        free(content);
        return NULL;
    }
    size_t num_ints = bytesRead / sizeof(uint8_t);
    uint8_t* values = malloc(num_ints * sizeof(uint8_t));
    
    if (values == NULL) {
        perror("Failed to allocate memory for integer array");
        free(content);
        return NULL;
    }

    // Safely copy and unpack the raw bytes into the integer array
    for (size_t i = 0; i < num_ints; i++) {
        memcpy(&values[i], content + (i * sizeof(uint8_t)), sizeof(uint8_t));
    }

    // Free the raw file buffer since we no longer need it
    free(content);

    // Pass back the total count of integers via output parameter
    if (outNumInts != NULL) {
        *outNumInts = num_ints;
    }

    // Return the valid pointer to the copied integer data
    return values;
}

int main() {
    size_t num_ints = 0;
    uint8_t* numbers = readBinFromFile("./source/res/images/aksenapati_snow_mountain", &num_ints);

    if (numbers != NULL) {
        printf("Successfully read %zu integers:\n", num_ints);
        for (size_t i = 0; i < num_ints; i++) {
            printf("Int %zu: %d\n", i, numbers[i]);
        }

        // Always free the allocated memory when finished
        free(numbers);
    }

    return 0;
}