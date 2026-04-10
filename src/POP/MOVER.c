#include "POP/eq.h"
#include "POP/gameeq.h"
#include "POP/seqdata.h"
#include "POP/movedata.h"
#include "POP/soundnames.h"

#include <stdint.h>

void animtrans() {
    trobcount = 0;
    if (!numtrans) return;

    int cleanflag = 0;

    do {
        tempnt = numtrans;
        animobj(numtrans);
        if (trdirec[0] < 0) { cleanflag = 1;}
        trdirec[tempnt] = cleanflag ? -1 : trdirec[0];
    } while (tempnt != 0);

    if (!cleanflag) {return;}

    int x, y;
    x = 1;
    y = 0;

    do {
        if (trdirec[x] != -1){
            trdirec[y] = trdirec[x];
            y++;
            trloc[y] = trloc[x];
            trscrn[y] = trscrn[x];
        }
        x++;
    } while(x <= numtrans);

    numtrans = y;
    return;
}

void animobj(uint8_t x) {
    trloc[0] = trloc[x];
    trscrn[0] = trscrn[x];
    trdirec[0] = trdirec[x];

    calcblue(trscrn[0]);

    state = BlueSpec[trloc[0]];

    switch (BlueType[trloc[0]] && idmask) {
        case (OBJ_TORCH):
            animtorch();
            break;
        
        default:
            stopobj();
    }

    BlueSpec[trloc[0]] = state;
}

void animtorch() {
    if (trdirec[0] < 0) {return;}

    if (trscrn[0] != VisScrn) { /* purge */ }

    state = getflameframe();

    /* redtorch */
}

uint8_t getflameframe() {
    
    rnd();
}