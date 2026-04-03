#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

/**
 * Structure of Level
 * blue_type and blue_spec contains the type and the state of a block
 * link_lok an link_map contains information about the interactive element of the map
 * map contains the screen neighbour of each screen 
 * character_position contains the data the position of caracters,
 * the first positions are dedicated to child
 * 
 */
typedef struct {
    uint8_t blue_type[720];
    uint8_t blue_spec[720];
    uint8_t link_lok[256];
    uint8_t link_map[256];
    struct map_entry {
        uint8_t left;
        uint8_t right;
        uint8_t up;
        uint8_t down;
    } map[24];
    uint8_t unused[64];
    uint8_t positions[192];
} Level;

#endif