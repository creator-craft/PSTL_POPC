#ifndef FRAMEADV_H
#define FRAMEADV_H

#include <stdint.h>

#define ORG_ADDR 0x1290

enum OBJECT_TYPE {
  OBJ_SPACE     = 0,
  OBJ_FLOOR     = 1,
  OBJ_BLOCK     = 2,
  OBJ_GATE      = 3,
  OBJ_SPIKES    = 4,
  OBJ_LOOSE     = 5,
  OBJ_TORCH     = 6,
  OBJ_EXIT      = 7,
  OBJ_SLICER    = 8,
  OBJ_FLASK     = 9,
  OBJ_PANELWIF  = 10,
  OBJ_PANELWOF  = 11,
  OBJ_PILLARTOP = 12
};

enum ENTITY_TYPE {
  TYPE_KID     = 1,
  TYPE_SHAD    = 2,
  TYPE_REFLECT = 3,
  TYPE_GD      = 4,
  TYPE_FF      = 5,
  TYPE_SWORD   = 6,
  TYPE_COMIX   = 7
};

// ----- GLOBALS -----
extern uint8_t index;
extern uint8_t rowno;  // 0 = top, 2 = bottom
extern uint8_t colno;  // 0 = left, 9 = right
extern uint8_t yindex; // block # (0-29)
extern uint8_t objid;
extern uint8_t state;
extern uint8_t Ay;
extern uint8_t Dy;
extern uint8_t gateposn;
extern uint8_t gatebot;
extern uint8_t xsave;
extern uint8_t blockxco;
extern uint8_t switches;
extern uint8_t obj1;
extern uint8_t obj2;
extern uint8_t blockthr;

// GAME ENGINE STATES
extern uint8_t SCRNUM;
extern uint8_t scrnLeft, scrnRight, scrnAbove, scrnBelow, scrnBelowL;
extern uint8_t genCLS;     // Clear screen flag
extern uint8_t PRECED, spreced;
extern uint8_t PREV[3], sprev[3];
extern uint8_t BELOW[10], SBELOW[10];

// ----- ROUTINES -----

/**
 * SURE : Draws the entier screen (10*3 blocks) from scratch.
 */
void SURE(void);

/**
 * FAST : Fast screen redraw.
 * Only updates the blocks presents in a refresh buffer.
 */
void FAST(void);

/**
 * GETINITOBJ : Get thhe initial state of an object.
 */
uint8_t GETINITOBJ(uint8_t y_index);

// ----- drawing -----

void RedBlockSure(void);
void RedDSure(void);
void RedBlockFast(void);
void RedDFast(void);

// Displayed sections (A = up, B = left, C = corner, D = down/floor)
void drawa(void);
void drawb(void);
void drawc(void);
void drawd(void);

// Movable elements
void drawma(void);
void drawmb(void);
void drawmc(void);
void drawmd(void);

// Specific elements
void drawfrnt(void);   // Frontpiece (foreground)
void drawfloor(void);
void drawhalf(void);
void drawobjs(void);

// Specific objects
void drawslicerf(void);
void drawgatebf(void);
void drawgateb(void);
void drawgatec(void);
void drawspikea(void);
void drawspikeb(void);
void drawtorchb(void);
void drawflaska(void);
void drawsworda(void);
void drawexitb(void);

// ----- utilities -----

void setback(void);
void setmid(void);
void getprev(void);
void getbelow(void);
void calcblue(uint8_t screen_num);
uint8_t getobjid(void);
uint8_t getobjid1(uint8_t screen_num, uint8_t y_idx);

void sortlist(void); // Sorts objects from background to foreground
void loadobj(uint8_t obj_index);
void wipesq(void);

#endif /* FRAMEADV_H */