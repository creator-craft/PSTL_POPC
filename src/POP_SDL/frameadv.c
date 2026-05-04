#include <stdio.h>

#include "POP_SDL/bgdata.h"
#include "POP_SDL/frameadv.h"
#include "POP_SDL/level.h"
#include "POP_SDL/eq.h"

defu8 torchflame[] = {0x52, 0x53, 0x54, 0x55, 0x56, 0x61, 0x62, 0x63, 0x64,
                       0x52, 0x54, 0x56, 0x63, 0x61, 0x55, 0x53, 0x64, 0x62};

int PRECED, spreced;
int BELOW[10], SBELOW[10];

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
        else if (op == ORA)
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
  // if (PRECED == piece_panelwif || PRECED == piece_panelwof ||
  //     PRECED == piece_pillartop || PRECED == piece_block)
  //   add_check(maska[objid], blockxco, Dy, AND);

  int texture_idx = PRECED == piece_archtop1 && objid == piece_panelwof
                        ? archpanel
                        : piecea[objid];

  int y_off = pieceay[objid];
  add_check(texture_idx - 1, blockxco, Dy + y_off, ORA); // Dy - 3 ??
}

void drawma(int col, int row, int objid) {
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
  printf("Not implemented drawb_stripe\n");

  // int texture_id = bstripe[PRECED];
  // add_check(texture_id - 1, blockxco, Dy - 32, OR);
}

void drawtorchb(int blockxco, int Ay) {
  add(torchflame[spreced], blockxco + 10, Ay - 45, ORA);
}

void drawb(int blockxco, int Dy, int objid) {
  if (objid == piece_block)
    return;

  if (PRECED == piece_space) { // :space
    if (spreced >= numpans + 1)
      return;
    int texture_id = spaceb[spreced];
    add_check(texture_id - 1, blockxco, Dy + spaceby[spreced], ORA);
  } else if (PRECED == piece_floor) { // :floor
    int texture_id = floorb[spreced < numpans + 1 ? spreced : 0];
    add_check(texture_id - 1, blockxco,
              Dy + floorby[spreced < numpans + 1 ? spreced : 0], ORA);
  } else if (PRECED == piece_block) { // :block
    int texture_id = blockb[spreced < numblox ? spreced : 0];
    add_check(texture_id - 1, blockxco, Dy + pieceby[PRECED], ORA);

    add_check(0, blockxco - 1, Dy - 61, ORA);
  } else if (pieceb[PRECED] == 0) { // :stripe
    drawb_stripe(blockxco, Dy);
  } else if (pieceb[PRECED] == panelb0) { // :panel
    if (spreced < numpans)
      add_check(panelb[spreced] - 1, blockxco, Dy + pieceby[PRECED], ORA);
  } else {
    add_check(pieceb[PRECED] - 1, blockxco, Dy + pieceby[PRECED], ORA);
    drawb_stripe(blockxco, Dy);
  }
}

void drawmb(int blockxco, int Ay, int objid) {
  if (PRECED == piece_torch)
    drawtorchb(blockxco, Ay);
}

void domaskb(int blockxco, int Dy) {
  int texture_id = maskb[PRECED];
  add_check(texture_id - 1, blockxco, Dy, AND); // blockxco is supposed
}

int checkc(int objid) {
  return objid == 0 || objid == piece_pillartop || objid == piece_panelwof ||
         objid >= piece_archtop1;
}

void dodrawc(int colno, int blockxco, int Dy) {
  int x = BELOW[colno], texture_id;
  if (BELOW[colno] == piece_block) { // :block
    int state = SBELOW[colno];
    texture_id = blockc[state < numblox ? state : 0];
  } else {
    texture_id = piecec[BELOW[colno]];
    if (texture_id == panelc0) { // :panel
      int state = SBELOW[colno];
      if (state >= numbpans)
        return;
      texture_id = panelc[state];
    }
  }

  add_check(texture_id - 1, blockxco, Dy, ORA);
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
              objid == piece_panelwof ? ORA : STA);
  }
}

void addfore(int tex_id, int x, int y, int op) { add_check(tex_id, x, y, op); }

void maddfore(int tex_id, int x, int y) {
  addfore(tex_id, x, y, AND); // MASK
  addfore(tex_id, x, y, ORA);
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
}

void drawBlock(Level *lvl, int screen_idx, int col, int row) {
  int objid = lvl->blue_type[screen_idx * 30 + row * 10 + col] & 0x1F;
  int state = lvl->blue_spec[screen_idx * 30 + row * 10 + col];

  printf("%d: %d\n", objid, state);

  if (row < 2) {
    BELOW[col] = lvl->blue_type[screen_idx * 30 + (row + 1) * 10 + col] & 0x1F;
    SBELOW[col] = lvl->blue_spec[screen_idx * 30 + (row + 1) * 10 + col];
  } else { // TODO: check down
    BELOW[col] =
        lvl->blue_type[(lvl->map[screen_idx].down - 1) * 30 + col] & 0x1F;
    SBELOW[col] = lvl->blue_spec[(lvl->map[screen_idx].down - 1) * 30 + col];
  }

  if (col == 0) { // TODO: check left
    PRECED =
        lvl->blue_type[(lvl->map[screen_idx].left - 1) * 30 + row * 10 + 9] &
        0x1F;
    spreced =
        lvl->blue_spec[(lvl->map[screen_idx].left - 1) * 30 + row * 10 + 9];
  }

  int blockxco = col * 32, Dy = row * 64 + 63;

  printf("%d preced: %d\n", objid, PRECED);
  drawc(col, blockxco, Dy, objid);
  drawb(blockxco, Dy, objid);
  drawmb(blockxco, Dy, objid);
  drawd(blockxco, Dy - 1, objid, state);
  drawa(blockxco, Dy, objid);
  drawma(blockxco, Dy, objid);
  drawfrnt(blockxco, Dy - 3, objid, state);

  PRECED = objid;
  spreced = state;
}