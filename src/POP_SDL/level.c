#include "POP_SDL/level.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int loadLevel(Level *level, char *filename) {
  FILE *f = fopen(filename, "r");

  if (f == NULL)
    return 1;

  size_t size = fread(level, sizeof(Level), 1, f);
  printf("s: %d %d\n", size, sizeof(Level));

  if (size != 1)
    return size;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 10; col++)
      printf("%02X:%02X ", level->blue_type[row * 10 + col] & ~0x1F,
             level->blue_type[row * 10 + col] & 0x1F);
    printf("\n");
  }

  return 0;
}
