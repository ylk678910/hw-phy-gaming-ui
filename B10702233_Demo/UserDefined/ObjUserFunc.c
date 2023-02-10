#include "main.h"
#include "ObjUserFunc.h"

ObjectGroup_t *BallGroup_Orange, *BallGroup_Blue;
ObjectPicture_t Picture_Orange, Picture_Blue;

uint8_t bTS_Debounce(void)
{
    TS_StateTypeDef TS_State;
    static uint8_t key = 0, pre_key = 0, debounce = 0;

    BSP_TS_GetState(&TS_State); //TS_State.touchDetected

    key = TS_State.touchDetected;
    if (key != pre_key)
        debounce++;
    if (debounce > 3)
    {
        pre_key = key;
        debounce = 0;
        if (key == 1)
            return (uint8_t)1;
    }

    return (uint8_t)0;
}

void vTS_PressControl(void (*fptr)(uint8_t, uint8_t, uint16_t, uint16_t))
{
    TS_StateTypeDef TS_State;
    BSP_TS_GetState(&TS_State); //TS_State.touchDetected

    uint16_t x, y;

    if (TS_State.touchDetected)
    {
        for (uint8_t TS_ptr = 0; TS_ptr < TS_State.touchDetected; TS_ptr++)
        {
            x = TS_State.touchX[TS_ptr];
            y = TS_State.touchY[TS_ptr];
            fptr(TS_ptr, TS_State.touchDetected, x, y);
        }
    }
    else
    {
        fptr(0, 0, 0, 0);
    }
}

void vDetectPress_NewDel(uint8_t Which, uint8_t Touchs, uint16_t x, uint16_t y)
{
		BSP_LCD_SetTextColor(BALL_COLOR_1);
		BSP_LCD_DisplayStringAt(5,5,(uint8_t*)"New",LEFT_MODE);
		BSP_LCD_SetTextColor(BALL_COLOR_2);
		BSP_LCD_DisplayStringAt(5+55,5,(uint8_t*)"New",LEFT_MODE);
		BSP_LCD_SetTextColor(BALL_COLOR_1);
		BSP_LCD_DisplayStringAt(5+55,5,(uint8_t*)"Del",RIGHT_MODE);
		BSP_LCD_SetTextColor(BALL_COLOR_2);
		BSP_LCD_DisplayStringAt(5,5,(uint8_t*)"Del",RIGHT_MODE);
		BSP_LCD_SetTextColor(TEXT_COLOR);
	
    static uint8_t Pressed_pre;

    if (Touchs)
    {
        if (Pressed_pre == 0)
        {
            if (y >= 5 && y <= 5 + 25)
            {
                if (x >= 5 && x <= 5 + 55) //Press Orange New 
                {
                    PyObj_t *Ball = NULL;
                    if (xNewPyObj(&Ball) == OBJECT_SUCCESS)
                    {
                        xObjectGroup_AddObj(BallGroup_Orange, PyObj_Obj(Ball));

                        xObjectType_ImportPic(PyObj_Obj(Ball), &Picture_Orange);
											
#ifndef USE_OVERLAPPING
												PyObj_Color(Ball) = BALL_COLOR_1;
#endif
												PyObj_Priority(Ball) = 1;
											
												//PyObj parameter setting
                        PyObj_Resistance(Ball) = 0.05;
                        PyObj_ReboundResis(Ball) = 0.8;
												PyObj_Restitution(Ball) = 0.7;
                        PyObj_Mode(Ball) = (NOT_EXCEED_BOUNDARY | EDGE_REBOUND_ACTIVE);

                        srand(ObjecTime);
                        xSetObjectTo(PyObj_Obj(Ball), rand() % (LAYOUT_WIDTH - PyObj_Width(Ball)), LAYOUT_HIGHT >> 1);
                        xSetVisible(PyObj_Obj(Ball), 1);
                    }
                }
                if (x >= 5 + 55 && x <= 5 + 2*55) //Press Blue New
                {
                    PyObj_t *Ball = NULL;
                    if (xNewPyObj(&Ball) == OBJECT_SUCCESS)
                    {
                        xObjectGroup_AddObj(BallGroup_Blue, PyObj_Obj(Ball));

                        xObjectType_ImportPic(PyObj_Obj(Ball), &Picture_Blue);
											
#ifndef USE_OVERLAPPING
												PyObj_Color(Ball) = BALL_COLOR_2;
#endif
												PyObj_Priority(Ball) = 0;

												//PyObj parameter setting
                        PyObj_Gravity(Ball) = 0.1;
                        PyObj_Resistance(Ball) = 0.05;
                        PyObj_ReboundResis(Ball) = 0.8;
												PyObj_Restitution(Ball) = 0.7;
                        PyObj_Mode(Ball) = (NOT_EXCEED_BOUNDARY | GRAVITY_ACTIVE | EDGE_REBOUND_ACTIVE);

                        srand(ObjecTime);
                        xSetObjectTo(PyObj_Obj(Ball), rand() % (LAYOUT_WIDTH - PyObj_Width(Ball)), LAYOUT_HIGHT >> 1);
                        xSetVisible(PyObj_Obj(Ball), 1);
                    }
                }
                if (x > -5 + 480 - 55*2 && x < -5 + 480- 55) //Press Orange Del
                {
                    PyObj_t *Ball;
                    if (xGetPyObjByObj(&Ball, xObjectGroup_GetObj(BallGroup_Orange, 0)) == OBJECT_SUCCESS)
                    {
                        xDeletePyObj(&Ball);
                    }
                }
                if (x > -5 + 480 - 55 && x < -5 + 480) //Press Blue Del
                {
                    PyObj_t *Ball;
                    if (xGetPyObjByObj(&Ball, xObjectGroup_GetObj(BallGroup_Blue, 0)) == OBJECT_SUCCESS)
                    {
                        xDeletePyObj(&Ball);
                    }
                }
            }
        }
        Pressed_pre = 1;
    }
    else
    {
        Pressed_pre = 0;
    }
}


#ifndef PYOBJ_DEFINED
void vDropBall_Group(ObjectGroup_t *Group)
{
    Object_t *ObjectNow;

    {
        uint8_t ptr = 0;
        ObjectNow = xObjectGroup_GetObj(Group, ptr);
        while (ObjectNow != NULL)
        {
            //CODE WRITE HERE START
            if (Object_Pressed(ObjectNow) == 0)
            {
                //Drop
                if (Object_yMax(ObjectNow) < (int)LAYOUT_HIGHT)
                {
                    xMoveObject(ObjectNow, 0, 1);
                }
            }
            //CODE WRITE HERE END
            ObjectNow = xObjectGroup_GetObj(Group, ++ptr);
        }
    }
}
#else
void vBallJump_Group(ObjectGroup_t *Group)
{
    PyObj_t *PyObjNow;
    Object_t *ObjectNow;

    {
        uint8_t ptr = 0;
        ObjectNow = xObjectGroup_GetObj(Group, ptr);
        while (ObjectNow != NULL)
        {
            xGetPyObjByObj(&PyObjNow, ObjectNow);
            //CODE WRITE HERE START
            if (PyObj_Pressed(PyObjNow) == 0)
            {
                xMovePyObj(PyObjNow);
            }
            //CODE WRITE HERE END
            ObjectNow = xObjectGroup_GetObj(Group, ++ptr);
        }
    }
    {
        uint8_t ptr = 0;
        ObjectNow = xObjectGroup_GetObj(Group, ptr);
        while (ObjectNow != NULL)
        {
            xGetPyObjByObj(&PyObjNow, ObjectNow);
            //CODE WRITE HERE START
            PyObj_Rebounded(PyObjNow) = 0;
            //CODE WRITE HERE END
            ObjectNow = xObjectGroup_GetObj(Group, ++ptr);
        }
    }
}
#endif
