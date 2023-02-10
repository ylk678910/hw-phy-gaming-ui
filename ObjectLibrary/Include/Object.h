#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "main.h"

//#include "../Include/main.h"

//#define DEBUG

#ifndef OBJ_DEFINED
#define  OBJ_DEFINED

/*********************************************/
/***************User Config Start*************/
/*********************************************/
#define USE_ARGB_PIC

#ifndef USE_ARGB_PIC
	//#define USE_OVERLAPPING     //Faster, but only can be used in single color mode(Use TEXT_COLOR).
#endif

#define LAYOUT_WIDTH 480  //Object x = 0 ~ LAYOUT_WIDTH-1
#define LAYOUT_HIGHT 272 //Object y = 0 ~ LAYOUT_HIGHT-1
#define TOP_BOUND 30
#define BOTTOM_BOUND LAYOUT_HIGHT
#define LEFT_BOUND 0  
#define RIGHT_BOUND LAYOUT_WIDTH 
#define BACKGROUND_COLOR (uint32_t)LCD_COLOR_BROWN
#define TEXT_COLOR (uint32_t)LCD_COLOR_ORANGE
#define GetBackgroundColor(XAbs, YAbs) (BACKGROUND_COLOR)

#define MAX_OBJECT 8
/*********************************************/
/****************User Config End**************/
/*********************************************/

#define TRANSPARANT_COLOR (uint32_t)0x00000000

////typedef
typedef enum
{
    X_Y_NEGATIVE_IGNORE = 0,
    X_Y_NEGATIVE_DO_NOT_IGNORE = 1
} IsXYNegativeIgnore_t;

#ifdef DEBUG
typedef enum
{
    SUCCESS = 0U,
    FAIL = !SUCCESS
} ErrorStatus;
#endif

typedef enum
{
    NEW,
    INITIAL,
    DEL,
    GET,
    GET_BY_MARK
} ObjectControl_t;

typedef enum
{
    AT_TOP,
    AT_BOTTOM,
    AT_RIGHT,
    AT_LEFT,
    NOT_FOUND,
    NOT_TOP,
    FOUND
} ObjectFindTop_t;

typedef enum
{
    OBJECT_SUCCESS,
    OBJECT_FAIL,
    OBJECT_TRUE,
    OBJECT_FALSE,
    CONTROL_STATE_WRONG,
    NO_MEMORY_SPACE,
    IS_Visible
} ObjectControlStatic_t;

typedef enum Mark_enum
{
    BALL1 = 0U,
    BALL2
} Mark_t;

typedef struct
{
    uint32_t Length, Width;
    uint32_t *Pic;
} ObjectPicture_t;

typedef struct
{
    int x, y;
    uint32_t Length, Width;
    uint32_t *Pic;
		uint32_t Color;						//If Color == 0 => Use the colors of the original image.
    uint32_t Num_of_PicSize;
    uint8_t Visible, Pressed;
    uint32_t Mark;
		uint32_t Priority;
} Object_t;
////

#ifdef DEBUG
void vDebugErrorPrint(ObjectControlStatic_t);
void xReadAllObject(void);
void DEBUGTEST(Object_t *OJ1, Object_t *OJ2, Object_t *OJ3);
#endif

#define Object_FindBig(a, b) ((a>b) ? a : b)
#define Object_FindSmall(a, b) ((a<b) ? a : b)

#define Object_x(Oj) ((Oj)->x)
#define Object_y(Oj) ((Oj)->y)
#define Object_xMax(Oj) ((Oj)->x + (int)(Oj)->Width)
#define Object_yMax(Oj) ((Oj)->y + (int)(Oj)->Length)
#define Object_xMid(Oj) ((Oj)->x + (int)(Oj)->Width/2)
#define Object_yMid(Oj) ((Oj)->y + (int)(Oj)->Length/2)
#define Object_Width(Oj) ((Oj)->Width)
#define Object_Length(Oj) ((Oj)->Length)
#define Object_Pic(Oj) ((Oj)->Pic)
#define Object_Color(Oj) ((Oj)->Color)
#define Object_Num_of_PicSize(Oj) ((Oj)->Num_of_PicSize)
#define Object_Visible(Oj) ((Oj)->Visible)
#define Object_Pressed(Oj) ((Oj)->Pressed)
#define Object_Mark(Oj) ((Oj)->Mark)
#define Object_Priority(Oj) ((Oj)->Priority)

extern volatile unsigned int ObjecTime;

void ObjectLCDSetPixel(uint16_t x, uint16_t y, uint32_t Color);

ObjectControlStatic_t xObjectData(Object_t **, ObjectControl_t, uint32_t);
ObjectControlStatic_t xInitObject(void);
ObjectControlStatic_t xNewObject(Object_t **);
ObjectControlStatic_t xGetObject(Object_t **, uint32_t);
ObjectControlStatic_t xGetObjectByMark(Object_t **, Mark_t);
ObjectControlStatic_t xDeleteObject(Object_t **);

ObjectControlStatic_t xOverlapPartPic(uint32_t **, const Object_t *, const Object_t *, const IsXYNegativeIgnore_t);
ObjectControlStatic_t xAllOverlapPartPic(uint32_t **, const Object_t *, const IsXYNegativeIgnore_t);

ObjectControlStatic_t xIsObjectOverlapping(const Object_t *ObjectSrc, const Object_t *ObjectCmp);
ObjectControlStatic_t xIsObjectOverlappingTP(const Object_t *ObjectSrc, const Object_t *ObjectCmp);
ObjectControlStatic_t xIsPicOverlapping(const uint32_t *, const uint32_t *, const int, const int, const uint32_t, const uint32_t, const int, const int, const uint32_t, const uint32_t);
ObjectControlStatic_t xIsPicOverlappingTP(const uint32_t *, const uint32_t *, const int, const int, const uint32_t, const uint32_t, const int, const int, const uint32_t, const uint32_t);
ObjectFindTop_t xIsOtherObjectAtPixel(const Object_t *, const uint32_t, const uint32_t);
ObjectControlStatic_t xFindObjectOverlapXYWL(uint32_t *, uint32_t *, uint32_t *, uint32_t *, const Object_t *, const Object_t *);
ObjectControlStatic_t xFindPicOverlapXYWL(uint32_t *, uint32_t *, uint32_t *, uint32_t *, const uint32_t *, const uint32_t *, const int, const int, const uint32_t, const uint32_t, const int, const int, const uint32_t, const uint32_t);
uint32_t uiFindObjectAtPixel(Object_t **ObjectFirst, Object_t **ObjectSecond, const uint32_t X_Abs, const uint32_t Y_Abs);

ObjectControlStatic_t xGenerateSameSizeEmptyPic(uint32_t **, const Object_t *);

ObjectControlStatic_t xObjectType_ImportPic(Object_t *, ObjectPicture_t *);
//Layout
ObjectControlStatic_t xSetObjectTo(Object_t *, const int, const int);
ObjectControlStatic_t xMoveObject(Object_t *, const int, const int);

//Visible
ObjectControlStatic_t xSetVisible(Object_t *, const uint8_t);
//

ObjectControlStatic_t xPointInObject(const Object_t *, const uint32_t, const uint32_t);

ObjectControlStatic_t DrawCircleToPicture(uint32_t **, uint32_t *, uint32_t *, const uint32_t);

uint32_t uiGetObjectPixel(const Object_t *, const uint32_t, const uint32_t);
uint32_t uiGetPicturePixel(const uint32_t *, const uint32_t, const uint32_t, const uint32_t, const uint32_t);
void vSetObjectPixel(Object_t *, const uint32_t, const uint32_t, const uint32_t);
void vSetPicturePixel(uint32_t *, const uint32_t, const uint32_t, const uint32_t, const uint32_t, const uint32_t);

ObjectFindTop_t ucIsTouchBound_LR(const Object_t *Object);
ObjectFindTop_t ucIsTouchBound_TB(const Object_t *Object);
#endif
