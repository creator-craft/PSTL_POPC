#include "POP/EQ.h"
#include "POP/BOOT.h"
#include "POP_external/AppleIIe.h"

// typedef void callable(void*);
#define callable void (*)(void)
#define CALL(func_addr) ((callable)func_addr)()

// @CALLABLE_ONCE @NORETURN
void entry() {
  ram_04FB = 0xFFFF;
  ram_03F3 = 0xFFFF;
  store_offset = 0xFF;
  ram_rd_main = 0xFF; // read?
  ram_wrt_main = 0xFF; // write?
  col_offset = 0xFF; // column?
  alt_charset_offset = 0xFF; // Alternative charset?

  text();
  home();
  pr0();
  in0();

  uint8_t rdsect = (((SLOT >> 3) | 0xC0) << 8) | 0x5C;

  uint8_t skewtbl[] = { 0x00, 0x0D, 0x0B, 0x09, 0x07, 0x05, 0x03, 0x01,
                        0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x0F };

  uint8_t sectaddr[] = { 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x30, 0x31, 0x32, 0x33, 0x34, 0x00, 0x00, 0x00 };

  for (sector = 0x0F; sector; sector++) {
    ram_003D = skewtbl[sector];
    uint8_t tmp = sectaddr[sector];

    if (tmp) {
      ram_0027 = tmp;
      CALL(rdsect);
    }
  }

  // a = SLOT ?? arg
  stage2();
}

void stage2() {
  // slot = x ??

  check128k();
  moverw18();

  lasttrack = 0;
  zero_reset_vector = 0;

  rw18();
  // 0x07, 0xA9

  rw18();
  // 0x00, 0x01, 0x00

  rw18();
  // 0x02, 0x00, 0x01

  rw18(); // Load & Run stage 3 boot from drive 1
  // 0xC3, 0xEE

  stage3();
}

uint8_t CHECKER() {
  static uint8_t ram_0800, ram_0C00, ram_C004, ram_C002;
  uint8_t tmp, ret = 1;

  set_ram_write_aux();
  set_ram_read_aux();
  ram_0800 = 0xEE;

  tmp = ram_0C00;
  if (tmp != 0xEE) {
    tmp = ram_0800;
    ram_0C00 <<= 1;
    if (tmp == ram_0C00)
      ret = 0;
  }
  ram_C004 = tmp; // set_ram_write_main();
  ram_C002 = tmp; // set_ram_read_main();
  return ret; // carry flag
}

void check128k() {
  static uint8_t ram_C081, ram_C017, ram_0180;

  ram_C081 = 0xFF; // a??

  if (apple_family_id != 6 || (ram_C017 & 0b10000000) == 0)
    NOT128K();

  // Move CHECKER to $0180
  if (CHECKER())
    NOT128K();
}

const char MEMTEXT[] = "\x8DREQUIRES A //C OR //E WITH 128K";
// @NORETURN
void NOT128K() {
  static uint8_t ram_C088[32];

  uint8_t a = ram_C088[SLOT];

  text();
  home();

  a = 8;
  vtab();

  for (int i = 0; MEMTEXT[i]; i++) {
    cout(MEMTEXT[i]);

    if (MEMTEXT[i] == 0x8D)
      ram_0024 = 4; // ??
  }

  while (1);
}

void moverw18() {
  read_write_ram_bank1();

  movemem(0xD0, 0x30, 0x40);
}

void movemem(uint8_t dst, uint8_t src, uint8_t src_end) {
  dest = (uint16_t)dst << 8;
  source = (uint16_t)src << 8;
  endsource = (uint16_t)src_end << 8; // Upcasts

  for (; source < endsource; source++)
    *(uint8_t*)dest = *(uint8_t*)source;
}