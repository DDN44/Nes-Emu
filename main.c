#include "stdio.h"
#include "memory.c"
#include "./cpu/cpu.c"
#include "./cpu/cpu.h"
#include "./ppu/ppu.c"
#include "./ppu/ppu.h"
#include "common.h"
#include <raylib.h>

const int screenWidth = 800;
const int screenHeight = 600;

int WinMain(int argc, char *argv[])
{
    SetTraceLogLevel(7);
    InitWindow(screenWidth, screenHeight, "Emulator");

    SetTargetFPS(5000);
    //if(argc < 2)
    //{
    //    printf("Input a file name please :)\n");
    //    return 1;
    //}

    //char *infile = argv[1];
    cpu_load_bin("bin.bin");
    cpu_init();
    pc = 0x8000;

    buffer = GenImageColor(800, 600, WHITE);
    textur = LoadTextureFromImage(buffer);
    while (!WindowShouldClose())
    {
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(textur, 0, 0, WHITE);
        DrawFPS(0, 0);
        EndDrawing();
        
        //printf("%d", GetKeyPressed());
        //getchar();
        if(IsKeyDown(265) | IsKeyPressed(264))
        {
            cpu_check_nmi();
            cpu_execute(cpu_read_mem(pc));
            //cpu_status();
        
            for(uint8_t i = 0; i < (cycles * 3); i++)
            {
                ppu_execute();
                ppu_draw();
            }
        }
    }

    CloseWindow();

    return 0;
}