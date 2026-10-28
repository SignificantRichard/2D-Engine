#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// a sheet of memory
typedef struct {
    uint32_t* page; // this keeps track of data
    uint16_t elements; // this keeps track of how many elements are in the page
    uint16_t size;
    uint16_t* addresses; // this will likely be very fragmented
    uint16_t availableAddress; // keep track of next available address (closest to 0)
    uint16_t addressSize;
    uint16_t lastAddress;
} Note;

// creates notepad with large size to prevent runtime fracture
Note createNotepad();
int resizeAllocation(Note* notepad, int size);
int resizeAddressAllocation(Note* notepad, int size);
void scanForLastElement(Note* notepad);
void scanForVacancy(Note* notepad);
uint16_t addElement(Note* notepad, uint32_t data);
void removeElement(Note* notepad, uint16_t location);
void defragElements(Note* notepad);

#endif