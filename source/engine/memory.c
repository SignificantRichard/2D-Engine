#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "memory.h"

/**
 * Creates a new Note struct with a large size to prevent runtime fracture.
 * 
 * @return A new Note struct with the specified size and initial capacity.
 */
Note createNotepad() {
    Note notepad;
    notepad.size = 1024; // Initial capacity of 1024 bytes
    notepad.page = malloc(notepad.size * sizeof(uint32_t)); // Allocate page memory
    notepad.elements = 0; // Initialize number of elements
    notepad.addressSize = notepad.size; // Set address size to page size
    notepad.addresses = malloc(notepad.size * sizeof(uint16_t)); // Allocate address memory
    
    // Initialize address array with zeros
    for (int i = 0; i < notepad.size; i++) {
        notepad.addresses[i] = 0xffff;
    }

    notepad.availableAddress = 0; // Initialize available address index

    return notepad;
}

/**
 * Resizes the memory allocation for a Note struct.
 * 
 * @param notepad The Note struct to resize.
 * @param size The new size of the memory allocation.
 * 
 * @return 0 if successful, 1 if allocation failed, 2 if memory would be overwritten.
 */
int resizeAllocation(Note* notepad, int size) {
    if (size < notepad->elements) {
        // If new size is smaller than current elements, warn user
        goto MEMORY_OVERRIDE_WARN;
    }

    uint32_t* temp = realloc(notepad->page, size * sizeof(uint32_t)); // Reallocate memory
    
    if (temp == NULL) {
        // If reallocation fails, print error message and return 1
        goto ALLOC_FAIL_WARN;
    }

    notepad->page = temp; // Update page memory
    notepad->size = size; // Update size

    return 0;

    ALLOC_FAIL_WARN:
    printf("Memory allocation failed\n");
    return 1;

    MEMORY_OVERRIDE_WARN:
    printf("Used memory would be overwritten\n");
    return 2;
}

/**
 * Resizes the address allocation for a Note struct.
 * 
 * @param notepad The Note struct to resize.
 * @param size The new size of the address allocation.
 * 
 * @return 0 if successful, 1 if allocation failed, 2 if memory would be overwritten.
 */
int resizeAddressAllocation(Note* notepad, int size) {
    if (size < notepad->elements) {
        // If new size is smaller than current elements, warn user
        goto MEMORY_OVERRIDE_WARN;
    }

    uint16_t oldSize = notepad->addressSize; // Store old address size

    uint16_t* temp = realloc(notepad->addresses, size * sizeof(uint16_t)); // Reallocate address memory
    
    if (temp == NULL) {
        // If reallocation fails, print error message and return 1
        goto ALLOC_FAIL_WARN;
    }

    notepad->addresses = temp; // Update address memory

    // Initialize address array with zeros
    for (int i = oldSize - 1; i < size; i++) {
        notepad->addresses[i] = 0xffff;
    }

    notepad->addressSize = size; // Update address size

    return 0;

    ALLOC_FAIL_WARN:
    printf("Memory allocation failed\n");
    return 1;

    MEMORY_OVERRIDE_WARN:
    printf("Used memory would be overwritten\n");
    return 2;
}

/**
 * Scans for the last element in the Note struct's address array.
 * 
 * @param notepad The Note struct to scan.
 */
void scanForLastElement(Note* notepad) {
    for (int i = 0; i < notepad->addressSize; i++) {
        if (notepad->addresses[i] != 0xffff) {
            notepad->lastAddress = i; // Store index of last element
        }
    }
}

/**
 * Scans for a vacant address in the Note struct's address array.
 * 
 * @param notepad The Note struct to scan.
 */
void scanForVacancy(Note* notepad) {
    for (int i = 0; i < notepad->addressSize; i++) {
        if (notepad->addresses[i] == 0xffff) {
            // If vacant address is found, store index and break
            notepad->availableAddress = i;
            break;
        }
    }
}

/**
 * Adds a new element to the Note struct's memory allocation.
 * 
 * @param notepad The Note struct to add to.
 * @param data The data to add.
 * 
 * @return The index of the newly added element.
 */
uint16_t addElement(Note* notepad, uint32_t data) {
    if (notepad->size <= 1 + (notepad->elements)) {
        // If new size is too small, resize allocation
        resizeAllocation(notepad, notepad->elements + 1);
    }
    notepad->page[notepad->elements] = data; // Store new element
    uint16_t addr = notepad->availableAddress; // Store address index
    notepad->addresses[addr] = notepad->elements; // Store index of newly added element
    scanForVacancy(notepad); // Scan for vacant address
    notepad->elements++; // Increment number of elements
    return addr;
}

/**
 * Removes an element from the Note struct's memory allocation.
 * 
 * @param notepad The Note struct to remove from.
 * @param location The index of the element to remove.
 */
void removeElement(Note* notepad, uint16_t location) {
    // Find index of element to remove
    uint16_t real = notepad->addresses[location];
    notepad->addresses[location] = 0xffff; // Wipe address
    notepad->page[real] = 0xffffffff; // Wipe page
}

/**
 * Defrags the elements in the Note struct's memory allocation.
 * 
 * @param notepad The Note struct to defrag.
 */
void defragElements(Note* notepad) {
    // Start at end of array
    scanForLastElement(notepad);

    for (int i = notepad->lastAddress; i >= 0; i--) {
        if (notepad->addresses[i] != 0xffff) {
            // Move element to start of array
            for (int j = 0; j < i; j++) {
                if (notepad->page[j] == 0xffffffff) {
                    notepad->page[j] = notepad->page[notepad->addresses[i]];
                    notepad->addresses[i] = j;
                    break;
                }
            }
        }
    }
}