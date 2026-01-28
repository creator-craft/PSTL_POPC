#ifndef APPLEIIE_H
#define APPLEIIE_H

void disable_80store(void);         // $C000
void enable_80store(void);          // $C001

void set_ram_read_main(void);       // $C002
void set_ram_read_aux(void);        // $C003

void set_ram_write_main(void);      // $C004
void set_ram_write_aux(void);       // $C005

void disable_80col(void);           // $C00C
void enable_80col(void);            // $C00D

void disable_alt_charset(void);     // $C00E
void enable_alt_charset(void);      // $C00F

// Bank Select Switches (page 83)
void read_ram_no_write_bank2(void);  // $C080
void read_rom_write_ram_bank2(void); // $C081 RR
void read_rom_no_write_bank2(void);  // $C082
void read_write_ram_bank2(void);     // $C083 RR
void read_ram_no_write_bank1(void);  // $C088
void read_rom_write_ram_bank1(void); // $C089 RR
void read_rom_no_write_bank1(void);  // $C08A
void read_write_ram_bank1(void);     // $C08B RR
void read_bank_2_or_1(void);         // $C011 R7
void read_ram_or_rom(void);          // $C012 R7
void use_main_bank(void);            // $C008 W
void use_aux_bank(void);             // $C009 W
void read_aux_or_main_bank(void);    // $C016 R7

#endif // APPLEIIE_H
