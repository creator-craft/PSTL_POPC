#include "POP_external/Level.h"
#include <stdint.h>

LEVEL *createLevel(char *level_file){
    LEVEL *level_res = malloc(sizeof(LEVEL));
    if (level_res == NULL){return NULL;}
    level_res->Map = malloc(24 * sizeof(level_res->Map));
    uint8_t *blue_type = malloc(720);
    uint8_t *blue_spec = malloc(720);
    uint8_t *linklok = malloc(256);
    uint8_t *linkmap = malloc(256);
    uint8_t *character_position = malloc(20);
    int err = (level_res->Map == NULL);
    err = err || !blue_type || !blue_spec;
    err = err || !linklok || !linkmap;
    err = err || !character_position;
    if (err) {return NULL;}
    level_res->BlueSpec = blue_spec;
    level_res->BlueType = blue_type;
    level_res->Character_Position = character_position;
    level_res->LinkLok = linklok;
    level_res->LinkMap = linkmap;
    return level_res;
}