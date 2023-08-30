#include <stdio.h>
#include "memory.c"
#include "./cpu/cpu.c"
#include "./cpu/cpu.h"
#include "./ppu/ppu.c"
#include "./ppu/ppu.h"
#include "common.h"
#include <raylib.h>

const int screenWidth = 256;
const int screenHeight = 240;

Color *pixels;
uint8_t frameout = 0;

//yes i know there is a lack of comments

int main(int argc, char *argv[])
{
    //SetTraceLogLevel(7);
    SetTargetFPS(60);
    window_init();
    //if(argc < 2)
    //{
    //    printf("Input a file name please :)\n");
    //    return 1;
    //}
    //char *infile = argv[1];
    cpu_load_bin("bin.bin");
    cpu_init();
    ppu_init();
    ppu_load_palette();
    pc = 0x8000;

    buffer = GenImageColor(256, 240, BLACK);
    textur = LoadTextureFromImage(buffer);
    
    BeginDrawing();
    ppu_init();
    DrawTexture(textur, 0, 0, WHITE);
    DrawFPS(0, 0);
    EndDrawing();

    while (!WindowShouldClose())
    {
        if(frameout == 1)
        {
            BeginDrawing();
            ppu_init();
            DrawTexture(textur, 0, 0, WHITE);
            DrawFPS(0, 0);
            EndDrawing();
        }

        //printf("%d", GetKeyPressed());
        //getchar();
        if(1/*IsKeyDown(265) | IsKeyPressed(264)*/)
        {
            cpu_check_nmi();
            cpu_execute(cpu_read_mem(pc));
            //cpu_status();
        
            for(uint8_t i = 0; i < (cycles * 3); i++)
            {
                ppu_execute();
            }
        }
    }
    

    CloseWindow();

    return 0;
}