#include <stdint.h>

#ifndef _common_h_
#define _common_h_
extern uint64_t cycles;
extern uint64_t ppucycles;
extern uint64_t scanlines;
extern uint8_t frameout;

const int screenWidth;
const int screenHeight;

extern uint8_t ppu_dma;
#endif