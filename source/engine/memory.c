// Memory currently only supports up to 32 bits
// If writing 0xffffffff, it will be marked as empty.
// Once elements larger than 32 bits are supported, this will not be a problem as
// 10 will mark head
// 01 will mark tail
// 00 will mark middle or no association
// 11 will never be used unless we want to mark this as empty

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "memory.h"

// creates notepad with large size to prevent runtime fracture
Note createNotepad() {
    Note notepad;
    notepad.size = 1024;
    notepad.page = malloc(notepad.size * sizeof(uint32_t));
    notepad.elements = 0;
    notepad.addressSize = notepad.size;
    notepad.addresses = malloc(notepad.size * sizeof(uint16_t));

    // wash addresses
    for (int i = 0; i < notepad.size; i++) {
        notepad.addresses[i] = 0xffff;
    }

    notepad.availableAddress = 0;

    return notepad;
}

int resizeAllocation(Note* notepad, int size) {
    if (size < notepad->elements) {
        goto MEMORY_OVERRIDE_WARN;
    }

    uint32_t* temp = realloc(notepad->page, size * sizeof(uint32_t));
    
    if (temp == NULL) {
        goto ALLOC_FAIL_WARN;
    }

    notepad->page = temp;
    notepad->size = size;

    return 0;

    ALLOC_FAIL_WARN:
    printf("Memory allocation failed\n");
    return 1;

    MEMORY_OVERRIDE_WARN:
    printf("Used memory would be overwritten\n");
    return 2;
}

int resizeAddressAllocation(Note* notepad, int size) {
    if (size < notepad->elements) {
        goto MEMORY_OVERRIDE_WARN;
    }

    uint16_t oldSize = notepad->addressSize;

    uint16_t* temp = realloc(notepad->addresses, size * sizeof(uint16_t));
    
    if (temp == NULL) {
        goto ALLOC_FAIL_WARN;
    }

    notepad->addresses = temp;

    // wash for free detection
    for (int i = oldSize - 1; i < size; i++) {
        notepad->addresses[i] = 0xffff;
    }

    notepad->addressSize = size;

    return 0;

    ALLOC_FAIL_WARN:
    printf("Memory allocation failed\n");
    return 1;

    MEMORY_OVERRIDE_WARN:
    printf("Used memory would be overwritten\n");
    return 2;
}

void scanForLastElement(Note* notepad) {
    for (int i = 0; i < notepad->addressSize; i++) {
        if (notepad->addresses[i] != 0xffff) {
            notepad->lastAddress = i;
        }
    }
}

void scanForVacancy(Note* notepad) {
    for (int i = 0; i < notepad->addressSize; i++) {
        if (notepad->addresses[i] == 0xffff) {
            // this is vacant
            notepad->availableAddress = i;
            break;
        }
    }
}

uint16_t addElement(Note* notepad, uint32_t data) {
    if (notepad->size <= 1 + (notepad->elements)) {
        resizeAllocation(notepad, notepad->elements + 1);
    }
    notepad->page[notepad->elements] = data;
    uint16_t addr = notepad->availableAddress;
    notepad->addresses[addr] = notepad->elements;
    scanForVacancy(notepad);
    notepad->elements++;
    return addr;
}

void removeElement(Note* notepad, uint16_t location) {
    // find where position is pointing to
    uint16_t real = notepad->addresses[location];
    notepad->addresses[location] = 0xffff; // wipe
    notepad->page[real] = 0xffffffff; // wipe
}

void defragElements(Note* notepad) {
    // start at end
    scanForLastElement(notepad);

    for (int i = notepad->lastAddress; i >= 0; i--) {
        if (notepad->addresses[i] != 0xffff) {
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