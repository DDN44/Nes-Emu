#include <stdint.h>
#include <stdio.h>
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

Color tempcolor;
uint8_t tempr = 0;
uint8_t tempg = 0;
uint8_t tempb = 0;
uint8_t current_color = 0;

Color palette[0x40] =
{
/*0x00*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x08*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x10*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x18*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x20*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x28*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, 
/*0x30*/  {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}, {0, 0, 0, 255}
};

void ppu_load_palette()
{
    uint8_t *buff;
    FILE *fp;
    uint8_t i = 0;
    fp = fopen("palette.pal", "r");
    while(i < 0x40)
    {
        buff = &tempr;
        fread(buff, 1, 1, fp);
        buff = &tempg;
        fread(buff, 1, 1, fp);
        buff = &tempb;
        fread(buff, 1, 1, fp);
        palette[i] = (Color){tempr, tempg, tempb, alpha};
        i++;
    }
    fclose(fp);
}

void ppu_init()
{
    pixels = buffer.data;
}

void window_init()
{
    InitWindow(screenWidth, screenHeight, "Emulator");
}

void ppu_execute()
{
    ppux++;
    if(ppux >= 256)
    {
        ppux = 0;
    }
    if(scanlines < 240)
    {
        pixels[ppux + (scanlines * buffer.width)] = palette[current_color];//(Color){val_r, val_g, val_b, alpha};
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

