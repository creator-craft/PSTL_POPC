#ifndef FRAMEADV_H
#define FRAMEADV_H

#include <stdint.h>

#include "POP_SDL/level.h"

extern uint8_t screen[320 * 192 / 8];
extern uint8_t BGTAB1[16384], BGTAB2[16384];

void drawBlock(Level *lvl, int screen_idx, int col, int row);

void drawTest();

#endif // FRAMEADV_H
