#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../memory.c"
#include "cpu.h"
#include "../common.h"

uint16_t pc;
uint8_t a;
uint8_t x;
uint8_t y;
uint8_t status;
uint8_t sp;
uint8_t nmi;

uint64_t cycles;

void cpu_init()
{
    uint16_t temp;
    temp = cpu_read_mem(0xFFFD);
    temp = temp << 8;
    pc = temp + cpu_read_mem(0xFFFC);
    a = 0;
    x = 0;
    y = 0;
    status = 0x36;
    sp = 0xFF;
    nmi = 0;
}

uint8_t cpu_disable_bit(uint8_t n, uint8_t disableshleft)
{   
    uint8_t temp = 0;
    if(disableshleft != 1)
    {
        temp = status << (7 - n);
    }else
    {
        temp = status;
    }
    temp = temp >> 7;
    if(temp == 1)
    {
        temp = status ^ (1 << n);
    }else
    {
        return status;
    }
    return temp;
}

void cpu_push(uint8_t z)
{
    cpu_write_mem(sp + 0x100, z);
    sp--;
}

void cpu_push16(uint16_t z)
{
    cpu_write_mem(sp + 0x100, z >> 8);
    sp--;
    cpu_write_mem(sp + 0x100, (z << 8) >> 8);
    sp--;
}

uint8_t cpu_pop()
{
    sp++;
    return cpu_read_mem(sp + 0x100);
}

uint16_t cpu_pop16()
{
    uint16_t temp;
    sp++;
    temp = cpu_read_mem(sp + 0x100);
    sp++;
    return ((cpu_read_mem(sp + 0x100) << 8) + temp);
}

uint16_t cpu_index_addr(uint8_t ind, uint8_t addr)
{
    addr += ind;
    return addr;
}

uint16_t cpu_index_addr16(uint8_t ind, uint16_t addr)
{
    addr += ind;
    return addr;
}

uint16_t cpu_op_addr()
{
    uint16_t atemp = cpu_read_mem(pc + 2);
    atemp = (atemp << 8) + cpu_read_mem(pc + 1);
    return atemp;
}

void cpu_save_addr(uint8_t reg)
{
    cpu_write_mem(cpu_op_addr(), reg);
}

uint16_t cpu_relative_addr()
{
    uint8_t rel = cpu_read_mem(pc + 1);
    uint8_t val;
    uint8_t neg;
    uint16_t ret;
    neg = rel >> 7;
    if(neg = 1)
    {
        val = ~rel + 1;
        ret = val;
        ret = (pc - ret) + 2;
    }else
    {
        val = rel;
        ret = val;
        ret = (pc + ret) + 2;
    }
    return ret;
}

void cpu_check_nmi()
{
    if(nmi == 1)
    {
        pc = (cpu_read_mem(0xFFFB) << 8) + cpu_read_mem(0xFFFA);
        nmi = 0;
    }
}

void cpu_zeropage(uint8_t var)
{
    cpu_write_mem(cpu_read_mem(pc + 1), var);
}

void cpu_zeropage_x(uint8_t var)
{
    uint16_t temp = x << 8;
    temp = temp + cpu_read_mem(pc + 1);
    cpu_write_mem(temp, var);
}

void cpu_zeropage_y(uint8_t var)
{
    uint16_t temp = y << 8;
    temp = temp + cpu_read_mem(pc + 1);
    cpu_write_mem(temp, var);
}

uint8_t cpu_asl(uint8_t var, uint8_t count)
{
    int8_t temp = var;
    temp = temp << count;
    var = temp;
    return var;
}

uint8_t cpu_asr(uint8_t var, uint8_t count)
{
    int8_t temp = var;
    temp = temp >> count;
    var = temp;
    return var;
}

void cpu_check_zero(uint8_t in)
{
    if(in == 0)
    {
        status = status | 0b00000010;
    }else
    {
        status = cpu_disable_bit(1, 0);
    }
}

void cpu_check_neg(uint8_t in)
{
    if(in >= 128)
    {
        status = status | 0b10000000;
    }else
    {
        status = cpu_disable_bit(7, 1);
    }
}

uint16_t cpu_indirectx()
{
    uint16_t atemp;
    atemp = cpu_read_mem(cpu_read_mem(pc + 1) + x + 1) << 8;
    atemp += cpu_read_mem(cpu_read_mem(pc + 1) + x);
    return atemp;
}

uint16_t cpu_indirecty()
{
    uint16_t atemp;
    uint16_t ytemp = y;
    atemp = cpu_read_mem(cpu_read_mem(pc + 1) + ytemp + 1) << 8;
    atemp += cpu_read_mem(cpu_read_mem(pc + 1) + ytemp);
    return atemp;
}

void cpu_execute(uint8_t op)
{
    switch(op)
    {
        case 0x00:
            cpu_push16(pc + 2);
            cpu_push(status);
            uint16_t tmppc;
            pc = (cpu_read_mem(0xFFFF) << 8) + cpu_read_mem(0xFFFE);
            cycles = 7;
            break;
        case 0x01:
            a |= cpu_read_mem(cpu_index_addr(x, cpu_read_mem(pc + 1)));
            cpu_check_zero(a);
            cpu_check_neg(a);
            break;
        case 0x05:
            a |= cpu_read_mem(cpu_read_mem(pc + 1));
            cpu_check_zero(a);
            cpu_check_neg(a);
            break;
        case 0x10:
            if((status >> 7) == 0)
            {
                pc = cpu_relative_addr();
            }else
            {
                pc += 2;
            }
            cycles = 2;
            break;
        case 0x20:
            cpu_push16(pc + 2);
            pc = cpu_op_addr();
            cycles = 6;
            break;
        case 0x30:
            if((status >> 7) == 1)
            {
                pc = cpu_relative_addr();
            }else
            {
                pc += 2;
            }
            cycles = 2;
            break;
        case 0x60:
            pc = cpu_pop16();
            pc++;
            cycles = 6;
            break;
        case 0x78:
            status = status | 0b00000100;
            pc++;
            cycles = 2;
            break;
        case 0x81:
            cpu_write_mem(cpu_indirectx(), a);
            pc += 2;
            cycles = 6;
            break;
        case 0x85:
            cpu_zeropage(a);
            pc += 2;
            cycles = 3;
            break;
        case 0x8D:
            cpu_save_addr(a);
            pc += 3;
            cycles = 4;
            break;
        case 0x91:
            cpu_write_mem(cpu_indirecty(), a);
            pc += 2;
            cycles = 6;
            break;
        case 0x95:
            cpu_write_mem(cpu_index_addr(x, cpu_read_mem(pc + 1)), a);
            pc += 2;
            cycles = 4;
            break;
        case 0x99:
            cpu_write_mem(cpu_index_addr16(y, cpu_op_addr()), a);
            pc +=3;
            cycles = 5;
            break;
        case 0x9A:
            sp = x;
            pc++;
            cycles = 2;
            break;
        case 0x9D:
            cpu_write_mem(cpu_index_addr16(x, cpu_op_addr()), a);
            pc +=3;
            cycles = 5;
            break;
        case 0xA0:
            y = cpu_read_mem(pc + 1);
            cpu_check_zero(y);
            cpu_check_neg(y);
            pc += 2;
            cycles = 2;
            break;
        case 0xA2:
            x = cpu_read_mem(pc + 1);
            cpu_check_zero(x);
            cpu_check_neg(x);
            pc += 2;
            cycles = 2;
            break;
        case 0xA9:
            a = cpu_read_mem(pc + 1);
            cpu_check_zero(a);
            cpu_check_neg(a);
            pc += 2;
            cycles = 2;
            break;
        case 0xAD:
            a = cpu_read_mem(cpu_op_addr());
            cpu_check_zero(a);
            cpu_check_neg(a);
            pc += 3;
            cycles = 4;
            break;
        case 0xC8:
            y += 1;
            cpu_check_zero(y);
            cpu_check_neg(y);
            cycles = 2;
            break;
        case 0xD0:
            if(((status << 6) >> 7) == 0)
            {
                pc += 2;
            }else
            {
                pc = cpu_relative_addr();
            }
            cycles = 2;
            break;
        case 0xD8:
            status = cpu_disable_bit(3, 0);
            pc++;
            cycles = 2;
            break;
    }
}

void cpu_status()
{
    printf("\r\r\r\r\r\r\r");
    printf("-----[Cpu Status]-----\n");
    printf("PC: %d, %x\n", pc, pc);
    printf("ACC: %d, %x\n", a, a);
    printf("X: %d, %x\n", x, x);
    printf("Y: %d, %x\n", y, y);
    printf("STATUS: %d, %x\n", status, status);
    printf("SP: %d, %x\n", sp, sp);
}