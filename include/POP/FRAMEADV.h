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
void SURE();

/**
 * FAST : Fast screen redraw.
 * Only updates the blocks presents in a refresh buffer.
 */
void FAST();

/**
 * GETINITOBJ : Get thhe initial state of an object.
 */
uint8_t GETINITOBJ(uint8_t y_index);

// ----- drawing -----

void RedBlockSure();
void RedDSure();
void RedBlockFast();
void RedDFast();

// Displayed sections (A = up, B = left, C = corner, D = down/floor)
void drawa();
void drawb();
void drawc();
void drawd();

// Movable elements
void drawma();
void drawmb();
void drawmc();
void drawmd();

// Specific elements
void drawfrnt();   // Frontpiece (foreground)
void drawfloor();
void drawhalf();
void drawobjs();

// Specific objects
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

// ----- utilities -----

void setback();
void setmid();
void getprev();

// Loads object ids and states of 10 blocks in the row below
void getbelow(uint8_t rowno);

// Returns the current screen's address
int calcblue();

// Returns the object id and state
uint8_t getobjid(uint8_t object_offset);
uint8_t getobjid1(uint8_t screen_num, uint8_t y_idx);

void sortlist(); // Sorts objects from background to foreground
void loadobj(uint8_t obj_index);
void wipesq();

#endif /* FRAMEADV_H */