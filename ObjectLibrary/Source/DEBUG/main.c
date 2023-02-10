#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "windows.h"
//#include "Object.h"
//#include "../Include/Object.h"
#include "../Include/main.h"
#include "../Source/Object.c"
#include "../Source/Simulation.c"

#ifdef DEBUG
//#include "../Include/Simulation.h"
#endif

int main(void)
{
#ifdef DEBUG
    InitLayout();
#endif
    xInitObject();
    Object_t *Ball1 = NULL, *Ball2 = NULL, *Ball3 = NULL;

    xNewObject(&Ball1);
    xNewObject(&Ball2);

    xObjectGetCirclePic(Ball1, 5);
    //xObjectGetCirclePic(Ball2, 6);
    printf("AAAAAAAAAAAAA");
    //Ball1->Mark = BALL1;
    //Ball2->Mark = BALL2;
    //
    //printf("Ball1 = %x\n", Ball1);
    //printf("Ball2 = %x\n", Ball2);
    //Object_t *Object_test;
    //Object_test = NULL;
    //xGetObjectByMark(&Object_test, BALL2);
    //printf("Ball2 = %x\n", Object_test);
    //Object_test = NULL;
    //xGetObjectByMark(&Object_test, BALL1);
    //printf("Ball1 = %x\n", Object_test);
    //Object_test = NULL;
    //xGetObjectByMark(&Object_test, 5);
    //printf("ERROR = %x\n", Object_test);
    //
    //system("PAUSE");

    //xSetObjectTo(Ball2, (LAYOUT_WIDTH) >> 1, LAYOUT_LENGTH >> 1);

    xSetObjectTo(Ball1, 61, 41);

    char in;
    printf("press enter");
    while (1)
    {
        scanf("%c", &in);
        system("cls");
        switch (in)
        {
        case '0':
            xMoveObject(Ball1, -1, -1);
            break;
        case '1':
            xMoveObject(Ball1, 1, 1);
            break;
        case '2':
            xSetVisible(Ball1, !Ball1->Visible);
            break;
        case '\n':
            PrintAll();
            break;
        }
    }
    system("PAUSE");
    return 0;
}
