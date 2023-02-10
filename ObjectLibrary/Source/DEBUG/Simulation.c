#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "windows.h"
#include "../Include/Simulation.h"

uint32_t *Layout;

void InitLayout(void)
{
    Layout = (uint32_t *)malloc(sizeof(uint32_t) * LAYOUT_LENGTH * LAYOUT_WIDTH);
    for (int y = 0; y < LAYOUT_LENGTH; y++)
    {
        for (int x = 0; x < LAYOUT_WIDTH; x++)
        {
            *(Layout + y * LAYOUT_WIDTH + x) = 0;
        }
    }
}

void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
    *(Layout + Ypos * LAYOUT_WIDTH + Xpos) = RGB_Code;
}

void PrintAll(void)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("   ");
    for (int x = 0; x < LAYOUT_WIDTH; x++)
    {
        printf("%d", x);
        if (x < 10)
        {
            printf(" ");
        }
    }
    printf("\n");

    for (int y = 0; y < LAYOUT_LENGTH; y++)
    {
        for (int x = 0; x < LAYOUT_WIDTH; x++)
        {
            if (y < 10 && x == 0)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                printf(" ");
            }
            if (x == 0)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                printf("%d ", y);
            }

            if (*(Layout + (y)*LAYOUT_WIDTH + x) == 1)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
                printf(" ");
                //printf("0");
            }
            else if (*(Layout + (y)*LAYOUT_WIDTH + x) == BACKGROUND_COLOR)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                printf("-");
            }
            else //COLORED
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE);
                printf("~");
            }
            printf(" ");
        }
        if (y != LAYOUT_LENGTH - 1)
            printf("\n");
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}