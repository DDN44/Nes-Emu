#include <stdint.h>
#include <raylib.h>

extern uint8_t *PPUCTRL;
extern uint8_t *PPUMASK;
extern uint8_t *PPUSTATUS;
extern uint8_t *OAMADDR;
extern uint8_t *OAMDATA;
extern uint8_t *PPUSCROLL;
extern uint8_t *PPUADDR;
extern uint8_t *PPUDATA;
extern uint8_t *OAMDMA;
extern Image buffer;
extern Texture2D textur;
extern Color *pixels;

Color tempcolor;
Color palette[0x40];
uint8_t tempr;
uint8_t tempg;
uint8_t tempb;
uint8_t current_color;

extern uint8_t *ppu_pt1;
extern uint8_t *ppu_pt2;

extern uint8_t *ppu_pal;

extern uint8_t ppu_oam[0x100];