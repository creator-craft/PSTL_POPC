#include <stdio.h>

#include "POP_SDL/bgdata.h"
#include "POP_SDL/frameadv.h"
#include "POP_SDL/level.h"

#define STA 1
#define OR 2
#define AND 3

int PRECED, spreced;

void add(int tex_id, int x, int y, int op) {
  uint8_t *images = tex_id >= 128 ? BGTAB2 : BGTAB1;
  tex_id %= 128;

  unsigned short offset = (((unsigned short)images[2 * tex_id + 2] << 8) |
                           (unsigned short)images[2 * tex_id + 1]) -
                          0x6000;

  unsigned char bytes_per_line = images[offset++],
                lines_count = images[offset++];

  for (int j = 0; j < lines_count; j++)
    for (int i = 0; i < bytes_per_line; i++) {
      int addr = ((y - j) * 320 + x) / 8 + i;
      if (addr >= 0 && addr < sizeof(screen))
        if (op == STA)
          screen[addr] = images[offset++];
        else if (op == OR)
          screen[addr] |= images[offset++];
        else if (op == AND)
          screen[addr] &= images[offset++];
    }
}

void add_check(int tex_id, int x, int y, int op) {
  if (tex_id >= 0)
    add(tex_id, x, y, op);
}

void drawa(int blockxco, int Dy, int objid) {
  if (PRECED == piece_panelwif || PRECED == piece_panelwof ||
      PRECED == piece_pillartop || PRECED == piece_block)
    add_check(maska[objid], blockxco, Dy, AND);

  int texture_idx = PRECED == piece_archtop1 && objid == piece_panelwof
                        ? archpanel
                        : piecea[objid];

  int y_off = pieceay[objid];
  add_check(texture_idx - 1, blockxco, Dy + y_off, OR); // Dy - 3 ??
}

void drawma(int col, int row, int objid) {
  // int mask_idx = maska[objid];
  // add_check(mask_idx - 1, col, row - 3 + pieceay[objid], AND);
}

void drawb_stripe(int blockxco, int Dy) {
  /*:stripe do EditorDisk
  lda #EditorDisk
  cmp #2
  beq :stripe
  fin

  lda BGset1
  cmp #1 ;pal
  bne ]rts*/
  printf("Not implemented drawb_stripe");

  // int texture_id = bstripe[PRECED];
  // add_check(texture_id - 1, blockxco, Dy - 32, OR);
}

void drawb(int blockxco, int Dy, int objid) {
  if (objid == piece_block)
    return;

  if (PRECED == piece_space) { // :space
    if (spreced >= numpans + 1)
      return;
    int texture_id = spaceb[spreced];
    add_check(texture_id - 1, blockxco, Dy + spaceby[spreced], OR);
  } else if (PRECED == piece_floor) { // :floor
    int texture_id = floorb[spreced < numpans + 1 ? spreced : 0];
    add_check(texture_id - 1, blockxco,
              Dy + floorby[spreced < numpans + 1 ? spreced : 0], OR);
  } else if (PRECED == piece_block) { // :block
    int texture_id = blockb[spreced < numblox ? spreced : 0];
    if (texture_id != 0)
      add_check(texture_id - 1, blockxco, Dy + pieceby[PRECED], OR);
    else { // :floor too
      texture_id = floorb[spreced < numpans + 1 ? spreced : 0];
      add_check(texture_id - 1, blockxco,
                Dy + floorby[spreced < numpans + 1 ? spreced : 0], OR);
    }
  } else if (pieceb[PRECED] == 0) { // :stripe
    drawb_stripe(blockxco, Dy);
  } else if (pieceb[PRECED] == panelb0) { // :panel
    if (spreced >= numpans || panelb[spreced] == 0)
      return;
    add_check(panelb[spreced] - 1, blockxco, Dy + pieceby[PRECED], OR);
  } else {
    add_check(pieceb[PRECED] - 1, blockxco, Dy + pieceby[PRECED], OR);
    drawb_stripe(blockxco, Dy);
  }
}

void drawmb(int col, int row, int objid) {
  // int mask_idx = maskb[objid];
  // if (mask_idx == 0)
  //   return;
  // add(mask_idx - 1, col, row - 3 + pieceby[objid], AND);
}

void domaskb(int blockxco, int Dy) {
  int texture_id = maskb[PRECED];
  add_check(texture_id - 1, blockxco, Dy, AND); // blockxco is supposed
}

int checkc(int objid) {
  return objid == piece_pillartop || objid == piece_panelwof ||
         objid >= piece_archtop1;
}

void dodrawc(int colno, int blockxco, int Dy) {
  // int x = BELOW[colno], texture_id;
  // if (BELOW[colno] == block) { // :block
  //   int state = SBELOW[colno];
  //   texture_id = blockc[state < numblox ? state : 0];
  // } else {
  //   texture_id = piecec[BELOW[colno]];
  //   if (texture_id == panelc0) { // :panel
  //     int state = SBELOW[colno];
  //     if (state >= numbpans)
  //       return;
  //     texture_id = panelc[state];
  //   }
  // }

  // if (texture_id)
  //   add(texture_id, blockxco, Dy, OR);
}

void drawc(int colno, int blockxco, int Dy, int objid) {
  if (!checkc(objid))
    return;

  dodrawc(colno, blockxco, Dy);
  domaskb(blockxco, Dy);
}

void drawd(int blockxco, int Dy, int objid, int state) {
  if (objid == blockb) { // :block
    if (blockd[state < numblox ? state : 0] == 0)
      printf("Should not appear\n");
    else
      add_check(blockd[state < numblox ? state : 0] - 1, blockxco, Dy, STA);
  } else {
    add_check(pieced[objid] - 1, blockxco, Dy,
              objid == piece_panelwof ? OR : STA);
  }
}

void addfore(int tex_id, int x, int y, int op) { add_check(tex_id, x, y, op); }

void maddfore(int tex_id, int x, int y) {
  addfore(tex_id, x, y, AND); // MASK
  addfore(tex_id, x, y, OR);
}

void DrawGateBF() {
  // if (rowno == KidBlockY && colno - 1 == KidBlockX && scrnRight != KidScrn)
  //   drawgatebf();
}

void drawslicerf(int blockxco, int Ay, int state) {
  int slicer_idx =
      slicerseq[(state & 0x7F) < slicerRet ? state & 0x7F : slicerRet];

  if (slicerfrnt[slicer_idx] != 0)
    maddfore(slicerfrnt[slicer_idx] - 1, blockxco, Ay);
}

void drawfrnt(int blockxco, int Ay, int objid, int state) {
  int texture_id = -1;
  if (PRECED == piece_gate)
    DrawGateBF();

  switch (objid) {
  case piece_slicer:
    drawslicerf(blockxco, Ay, state);
    break;
  case piece_flask:
    if (state & 0b11100000 == 0b10100000)
      break;
    if (state & 0b11100000 < 0b01000000)
      break;
    texture_id = specialflask;
  }

  if (texture_id == -1) // doesn't exist :1
    texture_id = fronti[objid];

  if (objid >= piece_archtop2) {
    addfore(texture_id - 1, blockxco + frontx[objid], Ay + fronty[objid], STA);
    return;
  }

  // TODO : EditorDisk (line 793-804)

  if (objid == piece_block)
    addfore(blockfr[state < numblox ? state : 0] - 1, blockxco + frontx[objid],
            Ay + fronty[objid], STA);
  else
    maddfore(texture_id - 1, blockxco + frontx[objid], Ay + fronty[objid]);

  // int texture_idx = fronti[objid];

  // if (texture_idx == 0)
  //   return;

  // int x_off = frontx[objid];
  // int y_off = fronty[objid];
  // add(texture_idx - 1, blockxco + x_off * 8, Ay - 3 + y_off, OR);
}

void drawBlock(Level *lvl, int screen_idx, int col, int row) {
  int objid = lvl->blue_type[screen_idx * 30 + row * 10 + col] & 0x1F;
  int state = lvl->blue_spec[screen_idx * 30 + row * 10 + col];

  printf("%d: %d\n", objid, state);

  if (col == 0) {
    PRECED =
        lvl->blue_type[lvl->map[screen_idx].left * 30 + row * 10 + col] & 0x1F;
    spreced = lvl->blue_spec[lvl->map[screen_idx].left * 30 + row * 10 + col];
  }

  int blockxco = col * 32, Dy = row * 64 + 63;

  printf("%d\n", objid);
  drawc(col, blockxco, Dy, objid);
  drawb(blockxco, Dy, objid);
  drawmb(blockxco, Dy, objid);
  drawd(blockxco, Dy, objid, state);
  drawa(blockxco, Dy, objid);
  drawma(blockxco, Dy, objid);
  drawfrnt(blockxco, Dy - 3, objid, state);

  PRECED = objid;
  spreced = state;
}