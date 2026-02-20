#include "POP_external/Level.h"
#include <stdint.h>
#include <stdio.h>

LEVEL *createLevel(char *level_file){
    LEVEL *level_res = malloc(sizeof(LEVEL));
    if (level_res == NULL){return NULL;}
    level_res->map = malloc(24 * sizeof(*(level_res->map)));
    uint8_t *blue_type = malloc(720);
    uint8_t *blue_spec = malloc(720);
    uint8_t *linklok = malloc(256);
    uint8_t *linkmap = malloc(256);
    uint8_t *character_position = malloc(20);
    int err = (level_res->map == NULL);
    err = err || !blue_type || !blue_spec;
    err = err || !linklok || !linkmap;
    err = err || !character_position;
    if (err) {return NULL;}
    level_res->blue_spec = blue_spec;
    level_res->blue_type = blue_type;
    level_res->character_position = character_position;
    level_res->link_lok = linklok;
    level_res->link_map = linkmap;
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
    s = fread(level->map, 1, 96, f);
    if (s < 96){return 1;}
    s = fread(level->character_position, 1, 20, f);
    level->c_p_length = s;
    return 0;
}

void destroyLevel(LEVEL *level){
    free(level->blue_spec);
    free(level->blue_type);
    free(level->character_position);
    free(level->link_lok);
    free(level->link_lok);
    free(level->link_map);
    free(level->map);
    free(level);
}