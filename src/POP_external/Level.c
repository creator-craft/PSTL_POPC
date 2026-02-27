#include "POP_external/Level.h"
#include <stdint.h>
#include <stdio.h>

LEVEL *createLevel(char *level_file){
    LEVEL *level_res = malloc(sizeof(LEVEL));
    if (level_res == NULL){return NULL;}
    return level_res;
}

int initiateLevel(LEVEL *level, char *level_file){
    FILE *f = fopen(level_file, 'r');
    if (f == NULL){return 1;}
    size_t s;
    s = fread(level->blue_type, 1, 720, f);
    if (s < 720){return 1;}
    s = fread(level->blue_spec, 1, 720, f);
    if (s < 720){return 1;}
    s = fread(level->link_lok, 1, 256, f);
    if (s < 256){return 1;}
    s = fread(level->link_map, 1, 256, f);
    if (s < 256){return 1;}
    s = fread(level->map, 4, 24, f);
    if (s < 96){return 1;}
    s = fread(level->character_position, 1, 20, f);
    level->c_p_length = s;
    return 0;
}

void destroyLevel(LEVEL *level){
    free(level);
}