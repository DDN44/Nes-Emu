#include <stdint.h>
#include <stdio.h>
#include "../memory.h"
#include "ppu.h"
#include "../cpu/cpu.h"
#include "../common.h"
#include <raylib.h>

uint8_t ppu_mem[0x4000];

uint8_t *PPUCTRL = &cpu_mem[0x2000];
uint8_t *PPUMASK = &cpu_mem[0x2001];
uint8_t *PPUSTATUS = &cpu_mem[0x2002];
uint8_t *OAMADDR = &cpu_mem[0x2003];
uint8_t *OAMDATA = &cpu_mem[0x2004];
uint8_t *PPUSCROLL = &cpu_mem[0x2005];
uint8_t *PPUADDR = &cpu_mem[0x2006];
uint8_t *PPUDATA = &cpu_mem[0x2007];
uint8_t *OAMDMA = &cpu_mem[0x4014];

uint8_t *ppu_pt1 = &ppu_mem[0x0000];
uint8_t *ppu_pt2 = &ppu_mem[0x1000];

uint8_t *ppu_nt1 = &ppu_mem[0x2000];
uint8_t *ppu_at1 = &ppu_mem[0x23C0];
uint8_t *ppu_nt2 = &ppu_mem[0x2400];

uint8_t *ppu_nt3 = &ppu_mem[0x2800];

uint8_t *ppu_nt4 = &ppu_mem[0x2C00];


uint8_t *ppu_pal = &ppu_mem[0x3F00];

uint8_t ppu_oam[0x100];

uint8_t ppu_dma = 0;

uint64_t ppucycles = 0;
uint64_t scanlines = 0;
uint64_t drawing = 0;
uint16_t current_tile = 0;

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
uint8_t current_index = 0;

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

uint8_t ppu_calc_pix()
{
    if(scanlines > 240)
    {
        return (uint8_t)0;
    }
    current_tile = ppu_nt1[x / 8];
    current_index = (((ppu_pt1[(current_tile << 4) + (scanlines % 8)] << (7 - (8 - (x % 8)))) >> 7) << 1) | ((ppu_pt1[(current_tile << 4) + (8 + (scanlines % 8))] << (7 - (8 - (x % 8)))) >> 7);
    if(current_index = 0)
    {
        return current_color;
    }else
    {
        return ppu_pal[current_index];
    }
}

void ppu_execute()
{
    current_color = ppu_calc_pix();
    if(ppu_dma == 1)
    {
        uint16_t dma_addr = (*OAMDMA << 8);
        uint8_t i = 0;
        while(i <= 0xFF)
        {
            ppu_oam[*OAMADDR + i] = cpu_read_mem(dma_addr + i);
            i++;
        }
        ppu_dma = 0;
    }
    ppu_oam[*OAMADDR] = *OAMDATA;
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

