#include <stdio.h>

#include "POP_SDL/frameadv.h"
#include "POP_SDL/bgdata.h"
#include "POP_SDL/level.h"

#define STA 1
#define OR 2

void add(int tex_id, int x, int y, int op) {
  uint8_t *images = tex_id >= 128 ? BGTAB2 : BGTAB1;
  tex_id %= 128;

  unsigned short offset = (((unsigned short)images[2 * tex_id + 2] << 8) |
                           (unsigned short)images[2 * tex_id + 1]) -
                          0x6000;

  unsigned char bytes_per_line = images[offset++],
                lines_count = images[offset++];

  for (int j = 0; j < lines_count; j++)
    for (int i = 0; i < bytes_per_line; i++)
      if ((y * 320 + j * 320 + x) / 8 + i >= 0 &&
          (y * 320 + j * 320 + x) / 8 + i < sizeof(screen))
        if (op == STA)
          screen[(y * 320 + j * 320 + x) / 8 + i] = images[offset++];
        else if (op == OR)
          screen[(y * 320 + j * 320 + x) / 8 + i] |= images[offset++];
}

void drawa(int col, int row, int objid) {
  int texture_idx = piecea[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int y_off = pieceay[objid];
  add(texture_idx, col * 32, row * 64 + y_off, OR);
}

void drawb(int col, int row, int objid) {
  int texture_idx = pieceb[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int y_off = pieceby[objid];
  add(texture_idx, col * 32, row * 64 + y_off, OR);
}

void drawc(int col, int row, int objid) {
  int texture_idx = piecec[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  add(texture_idx, col * 32, row * 64, OR);
}

void drawd(int col, int row, int objid) {
  int texture_idx = pieced[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  add(texture_idx, col * 32, row * 64, OR);
}

void drawfront(int col, int row, int objid) {

  int texture_idx = fronti[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int x_off = frontx[objid];
  int y_off = fronty[objid];
  add(texture_idx, col * 32 + x_off, row * 64 + y_off, OR);
}

void drawBlock(Level *lvl, int screen_idx, int col, int row) {
  int objid = lvl->blue_type[screen_idx * 30 + row * 10 + col] & 0x1F;

  printf("%d\n", objid);
  drawc(col, row, objid);
  drawb(col, row, objid);
  drawd(col, row, objid);
  drawa(col, row, objid);
  drawfront(col, row, objid);
}