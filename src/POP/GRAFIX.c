#include "POP/eq.h"

#include <stdint.h>


void rnd() {
    RNDseed = (5 * RNDseed + 23) % 256;
}