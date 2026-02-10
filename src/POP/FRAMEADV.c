#include "POP/FRAMEADV.h"

void SURE() {
  genCLS(1);

  setback();

  getprev();

  calcblue(SCRNUM);

  rowno = 2;

  do { // row
    Dy = BlockBot[rowno + 1];

    Ay = Dy - 3 - 1; // -1 with carry

    yindex = rowno * 10;

    PRECED = PREV[rowno]; // prev_object
    spreced = sprev[rowno]; // prev_state

    getbelow();

    colno = 0;
    do { // loop
      blockxco = XC0 = colno << 2; // X-coord for A-section

      objid = getobjid(); // yindex as argument

      RedBlockSure();

      PRECED = objid;
      spreced = state;

      yindex ++;
    } while (++colno <= 10);

  } while (rowno--);
  // rowno ++; // Not in original file (added to make a better loop)

  // NOW: draw bottom row of screen above

  rowno = 2;
  Dy = 2;
  Ay = -1;

  yindex = rowno * 10;

  PRECED = 0;
  push(scrnBelow);
  push(scrnBelowL);

  scrnBelow = SCRNUM;
  scrnBelowL = scrnLeft;

  getbelow();
  calcblue(scrnAbove);

  colno = 0;
  do { // dloop
    blockxco = CXO = colno << 2;

    if (scrnAbove || floor == 0) // TODO ???
      getobjid1(scrnAbove, yindex); // Supposed
    
    RedDSure();

    PRECED = objid;
    spreced = state;

    yindex ++;
  } while (++colno <= 10);

  scrnBelowL = pop();
  scrnBelow = pop();
}

void FAST();

uint8_t GETINITOBJ(uint8_t y_index);

void RedBlockSure();
void RedDSure();
void RedBlockFast();
void RedDFast();

void drawa();
void drawb();
void drawc();
void drawd();

void drawma();
void drawmb();
void drawmc();
void drawmd();

void drawfrnt();
void drawfloor();
void drawhalf();
void drawobjs();

void drawslicerf();
void drawgatebf();
void drawgateb();
void drawgatec();
void drawspikea();
void drawspikeb();
void drawtorchb();
void drawflaska();
void drawsworda();
void drawexitb();

void setback();
void setmid();
void getprev();
void getbelow();
void calcblue(uint8_t screen_num);
uint8_t getobjid();
uint8_t getobjid1(uint8_t screen_num, uint8_t y_idx);

void sortlist();
void loadobj(uint8_t obj_index);
void wipesq();
