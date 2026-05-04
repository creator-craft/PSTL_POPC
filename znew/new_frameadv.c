#include <stdio.h>

#include "POP_SDL/bgdata.h"
#include "POP_SDL/frameadv.h"
#include "POP_SDL/level.h"

#define STA 1
#define OR 2
#define AND 3
#define MASK 3

// TODO : warning with confusion between Dy and Ay

/*
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
*/

int PRECED, spreced; // TODO

enum Mode { MODE_ADDBACK, MODE_ADDMID } mode;

void add(int tex_id, int x, int y, int op) {
  if (mode == MODE_ADDBACK)
    addback(tex_id, x, y, op);
  else
    addmidez(tex_id, x, y, op); // UseFastlay is dead code ?
}

// ********** A SECTION ********** //

void addamask(int blockxco, int Dy, int objid) {
  int mask_idx = maska[objid];
  if (mask_idx == 0)
    return;

  add(mask_idx, blockxco, Dy + pieceay[objid], AND);
}

void adda1(int blockxco, int Dy, int IMAGE, int objid) {
  int y_off = pieceay[objid];
  add(IMAGE, blockxco, Dy + y_off, OR);
}

void adda(int blockxco, int Dy, int objid) {
  int texture_idx = piecea[objid]; // getpiecea

  if (texture_idx != 0)
    adda1(blockxco, Dy, texture_idx, objid);
}

void drawa(int blockxco, int Dy, int objid) {
  if (PRECED == panelwif || PRECED == panelwof || PRECED == pillartop ||
      PRECED == block)
    addamask(blockxco, Dy, objid);

  if (PRECED == archtop1 && objid == panelwof)
    adda1(blockxco, Dy, archpanel, objid);
  else
    adda(blockxco, Dy, objid);
}

// ********** B SECTION ********** //

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

  int texture_id = bstripe[PRECED];
  if (texture_id == 0)
    return;

  add(texture_id, blockxco, Dy - 32, OR);
}

void drawb(int blockxco, int Dy, int objid) {
  if (objid == block)
    return;

  if (PRECED == space) { // :space
    if (spreced >= numpans + 1)
      return;
    int texture_id = spaceb[spreced];
    if (texture_id)
      add(texture_id, blockxco, Dy + spaceby[spreced], OR);
  } else if (PRECED == floor) { // :floor
    int texture_id = floorb[spreced < numpans + 1 ? spreced : 0];
    if (texture_id)
      add(texture_id, blockxco,
          Dy + floorby[spreced < numpans + 1 ? spreced : 0], OR);
  } else if (PRECED == block) { // :block
    int texture_id = blockb[spreced < numblox ? spreced : 0];
    if (texture_id != 0)
      add(texture_id, blockxco, Dy + pieceby[PRECED], OR);
    else { // :floor too
      texture_id = floorb[spreced < numpans + 1 ? spreced : 0];
      if (texture_id == 0)
        add(texture_id, blockxco,
            Dy + floorby[spreced < numpans + 1 ? spreced : 0], OR);
    }
  } else if (pieceb[PRECED] == 0) { // :stripe
    drawb_stripe(blockxco, Dy);
  } else if (pieceb[PRECED] == panelb0) { // :panel
    if (spreced >= numpans || panelb[spreced] == 0)
      return;
    add(panelb[spreced], blockxco, Dy + pieceby[PRECED], OR);
  } else {
    add(pieceb[PRECED], blockxco, Dy + pieceby[PRECED], OR);
    drawb_stripe(blockxco, Dy);
  }
}

void domaskb(int blockxco, int Dy) {
  int texture_id = maskb[PRECED];
  if (texture_id)
    add(texture_id, blockxco, Dy, AND); // blockxco is supposed
}

// ********** C SECTION ********** //

int checkc(int objid) {
  return objid == pillartop || objid == panelwof || objid >= archtop1;
}

void drawc(int colno, int blockxco, int Dy, int objid) {
  if (!checkc(objid))
    return;

  dodrawc(colno, blockxco, Dy);
  domaskb(blockxco, Dy);
}

void dodrawc(int colno, int blockxco, int Dy) {
  int x = BELOW[colno], texture_id;
  if (BELOW[colno] == block) { // :block
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

  if (texture_id)
    add(texture_id, blockxco, Dy, OR);
}

// ********** D SECTION ********** //

void maddfore(int tex_id, int x, int y) {
  addfore(tex_id, x, y, MASK);
  addfore(tex_id, x, y, OR);
}

void redrawd(int blockxco, int Dy, int objid) {
  if (drawd(blockxco, Dy, objid))
    addfore();
}

int drawd(int blockxco, int Dy, int objid) {
  if (objid == block) { // :block
    if (blockd[state < numblox ? state : 0] == 0)
      drawa(blockxco, Dy, objid); // Secondary effect of bne (line 1123) ??
    else
      add(blockd[state < numblox ? state : 0], blockxco, Dy, STA);
  } else {
    int texture_id = pieced;
    if (texture_id)
      add(pieced[texture_id], blockxco, Dy, objid == panelwof ? OR : STA);
  }
}

// ********** FRONT SECTION ********** //

void drawfrnt(int blockxco, int Dy, int objid) {
  int texture_id = -1;
  if (PRECED == gate)
    DrawGateBF();

  switch (objid) {
  case slicer:
    drawslicerf();
    break;
  case flask:
    if (state & 0b11100000 == 0b10100000)
      break;
    if (state & 0b11100000 < 0b01000000)
      break;
    texture_id = specialflask;
  }

  if (texture_id == -1) // doesn't exist :1
    texture_id = fronti[objid];

  if (objid >= archtop2) {
    addfore(texture_id, blockxco + frontx[objid], Ay + fronty[objid], STA);
    return;
  }

  // TODO : EditorDisk (line 793-804)

  if (objid == block)
    addfore(blockfr[state < numblox ? state : 0], blockxco + frontx[objid],
            Ay + fronty[objid], STA);
  else
    maddfore(texture_id, blockxco + frontx[objid], Ay + fronty[objid]);
}

void DrawGateBF() {
  if (rowno == KidBlockY && colno - 1 == KidBlockX && scrnRight != KidScrn)
    drawgatebf();
}

// ********** OTHER ********** //

void drawBlock(Level *lvl, int screen_idx, int blockxco, int Dy) {
  int objid = lvl->blue_type[screen_idx * 30 + Dy * 10 + blockxco] & 0x1F;

  printf("%d\n", objid);
  drawc(blockxco, Dy, objid);
  drawb(blockxco, Dy, objid);
  // drawmb(blockxco, Dy, objid);
  drawd(blockxco, Dy, objid);
  drawa(blockxco, Dy, objid);
  // drawma(blockxco, Dy, objid);
  drawfront(blockxco, Dy, objid);
}

void drawAt(int x, int y) { screen[(y * 320 + x) / 8] |= 1 << (x % 8); }

void drawTest() {
  int blockxco = 0, Dy = 1, objid = 3;

  drawc(blockxco, Dy, objid);
  drawb(blockxco, Dy, objid);
  // drawmb(blockxco, Dy, objid);
  drawd(blockxco, Dy, objid);
  drawa(blockxco, Dy, objid);
  // drawma(blockxco, Dy, objid);
  drawfront(blockxco, Dy, objid);

  drawc(++blockxco, Dy, objid);
  drawb(++blockxco, Dy, objid);
  // drawmb(++blockxco, Dy, objid);
  drawd(++blockxco, Dy, objid);
  drawa(++blockxco, Dy, objid);
  // drawma(++blockxco, Dy, objid);
  drawfront(++blockxco, Dy, objid);

  drawAt(32, 60);
  drawAt(64, 60);
  drawAt(96, 60);
  drawAt(128, 60);
  drawAt(160, 60);
  drawAt(192, 60);
  drawAt(224, 60);
}