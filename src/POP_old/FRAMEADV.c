#include "POP/FRAMEADV.h"

void SURE() {
	genCLS = 1;

	setback();
	getprev();

	calcblue(SCRNUM);

	rowno = 2;

	for (rowno = 2; rowno >= 0; rowno--) { // row
		Dy = BlockBot[rowno + 1];
		Ay = Dy - 3 - 1; // -1 with carry

		yindex = rowno * 10;

		PRECED = PREV[rowno]; // prev_object
		spreced = sprev[rowno]; // prev_state

		getbelow();

		for (colno = 0; colno < 10; colno++) { // loop
			blockxco = XC0 = colno << 2; // X-coord for A-section

			objid = getobjid(); // yindex as argument
			RedBlockSure();

			PRECED = objid;
			spreced = state;

			yindex ++;
		}

	}

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

	for (colno = 0; colno < 10; colno++) { // dloop
		blockxco = CXO = colno << 2;

		if (scrnAbove || floor == 0)
			objid = getobjid1(scrnAbove, yindex); // Simplied because if floor is 0 then screnAbove is too
		else
			objid = floor;

		RedDSure();

		PRECED = objid;
		spreced = state;

		yindex ++;
	}

	scrnBelowL = pop();
	scrnBelow = pop();
}

void FAST() {
	getprev();

	calcblue(SCRNUM);

	redkidmeter = metbufs3(20, 0);
	redoppmeter = metbufs2(28, 0);

	yindex = 30;
	drawobjs();

	for (rowno = 2; rowno >= 0; rowno--) {
		Dy = BlockBot[rowno + 1];
		Ay = Dy - 3 - 1; // -1 with carry

		yindex = rowno * 10;
		PRECED = PREV[rowno];
		spreced = sprev[rowno];

		getbelow();

		for (colno = 0; colno < 10; colno++) {
			XCO = blockxco = colno << 2;

			// y = yindex
			objid = getobjid();
			RedBlockFast();

			PRECED = objid;
			spreced = state;

			yindex++;
		}
	}

	setback();

	rowno = 2;
	Dy = 2;
	Ay = -1;
	yindex = rowno * 10;
	PRECED = 0;

	uint8_t tempScrnBelow = scrnBelow;
	uint8_t tempScrnBelowL = scrnBelowL;

	scrnBelow = SCRNUM;
	scrnBelowL = scrnLeft;

	getbelow();

	if (scrnAbove != 0) {
		calcblue(scrnAbove);

		for (colno = 0; colno < 10; colno++) {
			XCO = blockxco = colno << 2;
			objid = getobjid1();

			RedDFast();

			PRECED = objid;
			spreced = state;

			yindex++;
		}
	}

	scrnBelowL = tempScrnBelowL;
	scrnBelow = tempScrnBelow;

	yindex = 0xFF;
	drawobjs();

	if (inbuilder == 0)
		updatemeters();
}

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
