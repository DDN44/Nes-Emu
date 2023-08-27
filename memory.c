#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#ifndef _6502_MEM_
#define _6502_MEM_
uint8_t cpu_mem[0x10000];

uint8_t cpu_read_mem(uint16_t addr)
{
    if(addr >= 0x0800 && addr <= 0x1FFF)
    {
        int base = addr % 0x800;
        return cpu_mem[base];
    }else
    {
    if(addr >= 0x2000 && addr <= 0x4000)
    {
        int base = 0x2000;
        int offset = addr % 8;
        return cpu_mem[base + offset];
    }else
    {
        return cpu_mem[addr];
    }
    }
}

void cpu_write_mem(uint16_t addr, uint8_t data)
{
    if(addr >= 0x0800 && addr <= 0x1FFF)
    {
        int base = addr % 0x800;
        cpu_mem[base] = data;
    }else
    {
    if(addr >= 0x2000 && addr <= 0x4000)
    {
        int base = 0x2000;
        int offset = addr % 8;
        cpu_mem[base + offset] = data;
    }else
    {
        cpu_mem[addr] = data;
    }
    }
}

void cpu_load_bin(char *text)
{
    uint8_t buff = 0;
    uint8_t *pbuff = &buff;
    FILE *fp;
    fp = fopen(text, "r");
    uint16_t i = 0;
    while(i < 0xFFFF)
    {
        fread(pbuff, 1, 1, fp);
        cpu_mem[i] = buff;
        i++;
    }
    fread(pbuff, 1, 1, fp);
    cpu_mem[i] = buff;
    fclose(fp);
}
#endif