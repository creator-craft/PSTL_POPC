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
typedef struct _level{
    uint8_t blue_type[720];
    uint8_t blue_spec[720];
    uint8_t link_lok[256];
    uint8_t link_map[256];
    struct {
        uint8_t left;
        uint8_t right;
        uint8_t up;
        uint8_t down;
    } map[24];
    uint8_t character_position[20];
    int c_p_length;
} LEVEL;

/**
 * Allocate the memory used for the structure LEVEL
 */
LEVEL* createLevel();

/**
 * initiate the attributes of the LEVEL structure with the data
 * contained in the file level_file
 * return 0 if the initialisation were well done
 * 1 else
 */
int initiateLevel(LEVEL *level, char *level_file);

/**
 * Free the memory use by the LEVEL level
 */
void destroyLevel(LEVEL *level);

#endif