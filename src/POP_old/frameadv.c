
#include <stdint.h>

void SURE() {

  for (int rowno = 2; rowno >= 0; rowno--) {
    for (int colno = 0; colno < 10; colno++) {
      RedBlockSure();
    }
  }

  // Draw bottom row of the screen above
  for (int colno = 0; colno < 10; colno++) {
    RedDSure();
  }
}

// Draw each layer of the block
void RedBlockSure() {
  // drawc();
  // drawmc();

  // drawb();
  // drawmb();

  // drawd();
  // drawmd();

  // drawa();
  // drawma();

  // drawfrnt();
}

void RedDSure() {
  // drawc();
  // drawmc();

  // drawb();
  // drawd();
  // drawmd();

  // drawfrnt();
}

// void drawma(uint8_t objid) {
//   switch (objid) {
//   case SPIKES:
//     drawspikea();
//     return;
//   case SLICER:
//     drawslicera();
//     return;
//   case FLASK:
//     drawflaska();
//     return;
//   case SWORD:
//     drawsworda();
//     return;
//   }
// }

// void drawspikea() {
//   add(spikea[(state & 0x80) ? SPIKEEXT : state], blockxco, Ay - 1, ORA);
// }

// void drawsworda() {
//   add((state == 1) ? SWORDGLEAM1 : SWORDGLEAM0, blockxco, Ay, STA);
// }