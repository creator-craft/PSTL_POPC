#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

/**
 * Structure of Level
 * BlueType and BlueSpec contains the type and the state of a block
 * LinkLok an LinkMap contains information about the interactive element of the map
 * Map contains the screen neighbour of each screen 
 * Character_Position contains the data the position of caracters,
 * the first positions are dedicated to child
 */
typedef struct _level{
    uint8_t *BlueType;
    uint8_t *BlueSpec;
    uint8_t *LinkLok;
    uint8_t *LinkMap;
    struct {
        uint8_t Left;
        uint8_t Right;
        uint8_t Up;
        uint8_t Down;
    } *Map;
    uint8_t *Character_Position;
} LEVEL;

/**
 * Allocate the memory, and initialyze the data of this level
 * with the data contained in the file level_file
 */
LEVEL* createLevel(char *level_file);

/**
 * Free the memory use by the LEVEL level
 */
void destroyLevel(LEVEL *level);

#endif