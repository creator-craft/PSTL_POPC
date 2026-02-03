#include "POP/FRAMEADV.h"

void SURE() {
  genCLS(1);

  setback();

  getprev();

  calcblue(SCRNUM);

  rowno = 2;

  Dy = BlockBot[rowno];

  Ay = Dy - 3; // - 1 ?? (carry)

  // Line 70
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
