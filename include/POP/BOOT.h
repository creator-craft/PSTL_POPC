#ifndef BOOT_H
#define BOOT_H

#include "stdint.h"

// ----- ROUTINES -----

void entry(); // $0801
void stage2(); // $0900

void check128k();
void NOT128K();
void moverw18();
void movemem(uint8_t dst, uint8_t src, uint8_t src_end);

#endif // BOOT_H