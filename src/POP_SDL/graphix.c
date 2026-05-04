#include <stdint.h>
#include "POP_SDL/eq.h"

struct bg_fg {
  uint8_t x, y, img, op;
};

struct mid {
  int8_t typ;
  uint8_t x, y, img, op, offset, table, cu, cd, cl, cr;
};

enum { maxback = 200, maxfore = 100, maxmid = 46 };
uint8_t bg_count, fg_count, mid_count;
static struct bg_fg bg[maxback], fg[maxfore];
static struct mid mid[maxmid];

// ...

void addback(int XCO, int YCO, int IMG, int OPACITY) {
  if (bg_count >= maxback || YCO >= 192)
    return;

  bg[bg_count++] = (struct bg_fg){XCO, YCO, IMG, OPACITY};
}

// void addfore(int IMG, int XCO, int YCO, int OPACITY) {
//   if (fg_count >= maxfore || YCO >= 192)
//     return;

//   fg[fg_count++] = (struct bg_fg){XCO, YCO, IMG, OPACITY};
// }

void addmid(int TYP, int XCO, int OFFSET, int YCO, int IMG, int TABLE,
            int OPACITY, int FCharFace, int FChar_CU, int FChar_CD,
            int FChar_CL, int FChar_CR) {
  // a = midTyp
  if (mid_count >= maxmid)
    return;

  mid[mid_count++] = (struct mid){
      TYP,     XCO,   YCO,      IMG,      OPACITY | ((FCharFace ^ 0xFF) & 0x80),
      OFFSET,  TABLE, FChar_CU, FChar_CD, FChar_CL,
      FChar_CR};
}

void addmidez(int TYP, int XCO, int YCO, int IMG, int TABLE, int OPACITY) {
  addmid(TYP, XCO, 0, YCO, IMG, TABLE, OPACITY, 0xFF, 0, 192, 0, 40);
}

void drawall(int blackflag) {
  dogen(1);

  if (blackflag == 0)
    sngpeel();
  zeropeel();

  drawwipe();
  drawback();
  drawmid();
  drawfore();
  drawmsg();
}

void dogen(int genCLS) {
  if (genCLS)
    cls();

  // ... purple copy-protection
}

void drawback() {
  for (int i = 0; i < bg_count; i++) {
    struct bg_fg e = bg[i];
    // setbgimg(e.img)
    fastlay(e.img, e.x, e.y, e.op);
  }
}

void drawmid() {
  for (int i = 0; i < mid_count; i++) {
    struct mid e = mid[i];
    // if (e.typ < 0)
    //   setcharimg();
    // else
    //   setbgimg();

    if ((e.typ & 0x7F) == 0) {
      fastlay(e.img, e.x, e.y, e.op);
    } else if ((e.typ & 0x7F) == 1) {
      lay(e.img, e.x, e.y, e.op, e.offset, e.cl, e.cr, e.cu,
          e.cd); // setadd1 + lay
    } else if ((e.typ & 0x7F) == 2) {
      layrsave();
      addpeel();
      lay(e.img, e.x, e.y, e.op, e.offset, e.cl, e.cr, e.cu,
          e.cd); // setadd1 + lay
    } else
      printf("Error ??");
  }
}

void drawfore() {
  for (int i = 0; i < fg_count; i++) {
    struct bg_fg e = fg[i];
    if (e.op == MASK)
      fastmask(e.img, e.x, e.y);
    else
      fastlay(e.img, e.x, e.y, e.op);
  }
}