#include "stdio.h"
#include "memory.c"
#include "./cpu/cpu.c"
#include "./cpu/cpu.h"
#include "./ppu/ppu.c"
#include "common.h"



int main(int argc, char *argv[])
{
    //if(argc < 2)
    //{
    //    printf("Input a file name please :)\n");
    //    return 1;
    //}

    //char *infile = argv[1];
    cpu_load_bin("bin.bin");
    cpu_init();
    pc = 0x8000;

    while(1 == 1)
    {
        //getchar();
        cpu_check_nmi();
        cpu_execute(cpu_read_mem(pc));
        //cpu_status();
        
        for(uint8_t i = 0; i < (cycles * 3); i++)
        {
            ppu_execute();
        }
    }

    return 0;
}