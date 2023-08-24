#include <stdint.h>
#include "ppu.h"
#include "../memory.h"
#include "../cpu/cpu.h"
#include "../common.h"
#include <raylib.h>

uint8_t *PPUCTRL = &cpu_mem[0x2000];
uint8_t *PPUMASK = &cpu_mem[0x2001];
uint8_t *PPUSTATUS = &cpu_mem[0x2002];
uint8_t *OAMADDR = &cpu_mem[0x2003];
uint8_t *OAMDATA = &cpu_mem[0x2004];
uint8_t *PPUSCROLL = &cpu_mem[0x2005];
uint8_t *PPUADDR = &cpu_mem[0x2006];
uint8_t *PPUDATA = &cpu_mem[0x2007];
uint64_t ppucycles = 0;
uint64_t scanlines = 0;
uint64_t drawing = 0;

uint16_t ppux = 0;
uint16_t ppuy = 0;

uint8_t val_r = 255;
uint8_t val_g = 0;
uint8_t val_b = 0;
uint8_t alpha = 255;

Image buffer;
Texture2D textur;
Color *pixels;
uint8_t frameout;

void ppu_init()
{
    pixels = buffer.data;
}

void ppu_execute()
{
    ppux++;
    if(ppux >= 256)
    {
        ppux = 0;
        ppuy++;
    }
    if(ppuy > 240)
    {
        ppuy = 0;
    }
    if(scanlines < 240)
    {
        pixels[ppux + (ppuy * buffer.width)] = (Color){val_r, val_g, val_b, alpha};
    }
    ppucycles++;
    if(scanlines > 260)
    {
        scanlines = 0;
    }
    if((ppucycles >= 341) && (scanlines >= 240))
    {
        ppucycles = 0;
        UpdateTexture(textur, pixels);
        frameout = 1;
    }
    if(ppucycles >= 341)
    {
        ppucycles = 0;
        scanlines++;
    }
    if((scanlines >= 240) && (scanlines <= 261))
    {
        *PPUSTATUS = *PPUSTATUS | 0b10000000;
    }
    if((*PPUSTATUS >> 7) && (*PPUCTRL >> 7))
    {
        nmi = 1;
    }
    
}

