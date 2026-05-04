//
// Created by OrianeCrouzet on 26/01/2025.
//

#include "boot.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//On définit les adresses mémoires comme des constantes
#define SLOT 0x2B
#define SECTOR 0x50
#define TEXT 0xFB2F
#define HOME 0xFC58
#define VTAB 0xFB5B
#define COUT 0xFDF0
#define NORMAL 0xFE84
#define PR0 0xFE93
#define IN0 0xFE89

#define RW18 0xD000
#define SLOT_ADDR 0xFD
#define TRACK_ADDR 0xFE
#define LAST_TRACK_ADDR 0xFF

#define DEST_ADDR 0x0000
#define SOURCE_ADDR 0x0002
#define END_SOURCE_ADDR 0x0004

//Tables de secteurs et de skew
const uint8_t skewtbl[] = {
    0x00, 0x0D, 0x0B, 0x09, 0x07, 0x05, 0x03, 0x01,
    0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x0F
};

const uint8_t sectaddr[] = {
    0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x00, 0x00, 0x00
};

//Texte utilisé ligne 190
const char memtext[] = "\x8DREQUIRES A //C OR //E WITH 128K\x00";

//Fonction qui teste qu'on a bien une mémoire 128K
uint8_t check128k() {
    uint8_t checker[] = {0xEE, 0x00, 0x01, 0x00};
    uint8_t* addr = (uint8_t*) 0xC005;
    *addr = checker[0];
    addr = (uint8_t*) 0xC003;
    *addr = checker[1];
    addr = (uint8_t*) 0x0800;
    *addr = checker[2];
    uint8_t value = *(uint8_t*) 0x0C00;
    if (value != checker[0]) {
        return 0;
    }
    value <<= 1;
    if (*(uint8_t*) 0x0800 != value) {
        return 0;
    }
    return 1;
}

// Fonction moverw18
void moverw18() {
    uint8_t* dest = (uint8_t*) DEST_ADDR;
    uint8_t* source = (uint8_t*) SOURCE_ADDR;
    uint8_t* endsourc = (uint8_t*) END_SOURCE_ADDR;
    uint16_t d0 = 0xD0;
    uint16_t x = 0x30;
    uint16_t y = 0x40;

    while (1) {
        uint8_t* src = source;
        uint8_t* dst = dest;
        for (uint16_t i = 0; i < y; i++) {
            *dst = *src;
            src++;
            dst++;
        }

        source += 0x100;
        dest += 0x100;

        if (source >= endsourc) {
            break;
        }
    }
}

//Fonction pour lire une adresse secteur
void read_sector(uint8_t sector) {
    uint8_t* addr = (uint8_t*) SECTOR;
    *addr = sector;

    uint8_t* rw18 = (uint8_t*) RW18;
    *rw18 = 0x07;

    addr = (uint8_t*) TRACK_ADDR;
    *addr = 0x01;

    addr = (uint8_t*) SECTOR;
    *addr = sector;

    uint8_t* buffer = (uint8_t*) 0x0000;
    for (uint16_t i = 0; i < 0x100; i++) {
        buffer[i] = *rw18;
    }
}

//Fonction pour boot le système
void boot() {
    uint8_t* slot = (uint8_t*) SLOT;
    *slot = 0x60;

    uint8_t* text = (uint8_t*) TEXT;
    *text = 0x01;
    uint8_t* home = (uint8_t*) HOME;
    *home = 0x01;

    uint8_t* normal = (uint8_t*) NORMAL;
    *normal = 0x01;
    uint8_t* pr0 = (uint8_t*) PR0;
    *pr0 = 0x01;

    uint8_t* in0 = (uint8_t*) IN0;
    *in0 = 0x01;

    if (!check128k()) {
        printf("%s\n", memtext);
        return;
    }

    moverw18();

    read_sector(0x01);

    uint8_t* boot_code = (uint8_t*) 0x900;
    boot_code[0] = 0x01;
}

/*int main() {
    boot();
    return 0;
}*/
