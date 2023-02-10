#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"

#ifdef DEBUG
#include "../Include/Object.h"
#include "../Include/Simulation.h"
#else
#include "Object.h"
#endif

static ObjectControlStatic_t vInLCDRange(uint16_t *X_Abs, uint16_t *Y_Abs, uint16_t *X_max, uint16_t *Y_max, int x, int y, int Length, int Width);
static uint8_t ucIsPixelTP(Object_t *Object, uint16_t X_Rel, uint16_t Y_Rel);

#ifdef DEBUG
//Print Error
void vDebugErrorPrint(ObjectControlStatic_t Error)
{
    if (Error != OBJECT_SUCCESS)
    {
        switch (Error)
        {
        case OBJECT_SUCCESS:
            printf("OBJECT_SUCCESS\n");
            break;
        case OBJECT_FAIL:
            printf("OBJECT_FAIL\n");
            break;
        case OBJECT_SUCCESS:
            printf("OBJECT_SUCCESS\n");
            break;
        case OBJECT_NOT_EMPTY:
            printf("OBJECT_NOT_EMPTY\n");
            break;
        case OBJECT_FAIL:
            printf("OBJECT_FAIL\n");
            break;
        case OBJECT_FAIL:
            printf("OBJECT_FAIL\n");
            break;
        case OBJECT_FAIL:
            printf("OBJECT_FAIL\n");
            break;
        case CONTROL_STATE_WRONG:
            printf("CONTROL_STATE_WRONG\n");
            break;
        case OBJECT_SUCCESS:
            printf("OBJECT_SUCCESS\n");
            break;
        case OBJECT_FAIL:
            printf("\nOBJECT_FAIL\n");
            break;
        case OBJECT_SUCCESS:
            printf("\nOBJECT_SUCCESS\n");
            break;
        case NO_MEMORY_SPACE:
            printf("\nNO_MEMORY_SPACE\n");
            break;
        case OBJECT_SUCCESS:
            printf("\nOBJECT_SUCCESS\n");
            break;
        case IS_Visible:
            printf("\nIS_Visible\n");
            break;
        }
    }
}

//Print all x, y in the object
void xReadAllObject(void)
{
    for (int ptr = 0; ptr < MAX_OBJECT - 1; ptr++)
    {
        Object_t *ObjectTemp;
        if (xGetObject(&ObjectTemp, ptr) != OBJECT_FAIL)
        {
            printf("Object[%d].x=%d, y=%d\n", ptr, ObjectTemp->x, ObjectTemp->y);
        }
    }
}

void DEBUGTEST(Object_t *OJ1, Object_t *OJ2, Object_t *OJ3)
{
    static int i = 0;
    i++;
    if (i == 9)
    {
        // printf("XX");
    }

    int a = rand() % 2;
    int b = rand() % 2;
    int c = rand() % 2;
    int a1 = rand() % 5;
    int b1 = rand() % 5;
    int c1 = rand() % 5;
    int d = rand() % 2;
    int e = rand() % 2;
    int f = rand() % 2;
    int d1 = rand() % 5;
    int e1 = rand() % 5;
    int f1 = rand() % 5;
    if (a)
    {
        a1 = -a1;
    }
    if (b)
    {
        b1 = -b1;
    }
    if (c)
    {
        c1 = -c1;
    }
    if (d)
    {
        d1 = -d1;
    }
    if (e)
    {
        e1 = -e1;
    }
    if (f)
    {
        f1 = -f1;
    }
    xMoveObject(OJ1, a1, d1);
    xMoveObject(OJ2, b1, e1);
    xMoveObject(OJ3, c1, f1);
#ifdef DEUG
    PrintAll();
#endif
}
#else

void ObjectLCDSetPixel(uint16_t x, uint16_t y, uint32_t Color)
{
	#ifndef USE_ARGB_PIC
    if (Color != BSP_LCD_ReadPixel(x, y))
    {
        BSP_LCD_DrawPixel(x, y, Color);
    }
	#else
    if ((Color & 0xFF000000) && Color != BSP_LCD_ReadPixel(x, y))
    {
        BSP_LCD_DrawPixel(x, y, Color);
    }
	#endif
}
#endif

ObjectControlStatic_t xPointInObject(const Object_t *ObjectSrc, const uint32_t X_Abs, const uint32_t Y_Abs)
{
    if (ObjectSrc == NULL)
        return OBJECT_FAIL;
    else
    {
        if (ObjectSrc->x + (int)ObjectSrc->Width > (int)X_Abs && ObjectSrc->y + (int)ObjectSrc->Length > (int)Y_Abs)
        {
            if (ObjectSrc->x <= (int)X_Abs && ObjectSrc->y <= (int)Y_Abs)
            {
                return OBJECT_SUCCESS;
            }
        }
    }
    return OBJECT_FAIL;
}

//Set X, Y pixel at Object
void vSetObjectPixel(Object_t *Object, const uint32_t RelX, const uint32_t RelY, const uint32_t SetClear)
{
    vSetPicturePixel(Object->Pic, Object->Width, Object->Length, RelX, RelY, SetClear);
}

//Set X, Y pixel at Picture
void vSetPicturePixel(uint32_t *Pic, const uint32_t Width, const uint32_t Length, const uint32_t RelX, const uint32_t RelY, const uint32_t SetClear)
{
#ifndef USE_ARGB_PIC
    uint32_t PixelLocation = Width * RelY + RelX;

    if (SetClear)
        *(Pic + (PixelLocation >> 5)) |= 1 << (31 - (PixelLocation & 31));
    else
        *(Pic + (PixelLocation >> 5)) &= ~(1 << (31 - (PixelLocation & 31)));
#else
    uint32_t PixelLocation = Width * RelY + RelX;

    *(Pic + PixelLocation) = SetClear;
#endif
}

//Get X, Y pixel at Object
uint32_t uiGetObjectPixel(const Object_t *Object, const uint32_t RelX, const uint32_t RelY)
{
		uint32_t Color = uiGetPicturePixel(Object->Pic, Object->Width, Object->Length, RelX, RelY);
		if(Object_Color(Object) & 0xFF000000)
		{
	#ifndef USE_ARGB_PIC
				 if(Color == TEXT_COLOR)
					 Color = Object_Color(Object);
	#else
				Color &= 0xFF000000;
				Color |= (Object_Color(Object) & 0x00FFFFFF);
	#endif
		}
		return Color;
}

//Get X, Y pixel at Picture
uint32_t uiGetPicturePixel(const uint32_t *Pic, const uint32_t Width, const uint32_t Length, const uint32_t RelX, const uint32_t RelY)
{
	#ifndef USE_ARGB_PIC
    uint32_t PixelLocation = Width * RelY + RelX;

    uint32_t PicData = *(Pic + (PixelLocation >> 5));
    if (PicData & (1 << (31 - (PixelLocation & 31))))
        return TEXT_COLOR; //32bit
    else
        return BACKGROUND_COLOR;
	#else
    uint32_t PixelLocation = Width * RelY + RelX;

    uint32_t PicData = *(Pic + PixelLocation);
    return PicData; //32bit
	#endif
}

ObjectControlStatic_t xSetVisible(Object_t *ObjectSrc, const uint8_t Visible)
{
    if (ObjectSrc->Visible != Visible)
    {
        ObjectSrc->Visible = Visible;
        xSetObjectTo(ObjectSrc, ObjectSrc->x, ObjectSrc->y);
    }
    return OBJECT_SUCCESS;
}

#ifndef PR
//Return if there is a Object expect for input at the x,y
ObjectFindTop_t xIsOtherObjectAtPixel(const Object_t *Objecr_Src, const uint32_t X_Abs, const uint32_t Y_Abs)
{
    Object_t *Object_temp;
    for (uint32_t ptr = 0; ptr < MAX_OBJECT; ptr++)
    {
        if (xGetObject(&Object_temp, ptr) == OBJECT_SUCCESS)
        {
            if (Object_temp != Objecr_Src)
            {
                if (Object_temp->Visible)
                {
                    if (X_Abs - Object_temp->x < Object_temp->Width && Y_Abs - Object_temp->y < Object_temp->Length)
                    {
#ifdef TP
                        if (!GetTP(Object_temp, X, Y))
                        {
#endif
                            if (uiGetObjectPixel(Object_temp, X_Abs - Object_temp->x, Y_Abs - Object_temp->y))
                            {
                                return FOUND;
                            }
#ifdef TP
                        }
#endif
                    }
                }
            }
        }
    }
    return NOT_FOUND;
}

//Return the first find Object at the x,y
uint32_t uiFindObjectAtPixel(Object_t **ObjectFirst, Object_t **ObjectSecond, const uint32_t X_Abs, const uint32_t Y_Abs)
{
    uint32_t PT_max = 0;
    uint32_t PT_sec = 0;
    *ObjectFirst = NULL;
    *ObjectSecond = NULL;

    Object_t *Object_temp;
    uint32_t Num = 0;

    for (uint32_t ptr = 0; ptr < MAX_OBJECT; ptr++)
    {
        if (xGetObject(&Object_temp, ptr) == OBJECT_SUCCESS)
        {
            if (Object_temp->Visible)
            {
                if ((int)X_Abs >= Object_temp->x && (int)X_Abs < Object_temp->x + (int)Object_temp->Width && (int)Y_Abs >= Object_temp->y && (int)Y_Abs < Object_temp->y + (int)Object_temp->Length)
                {
                    if (!ucIsPixelTP(Object_temp, X_Abs - Object_temp->x, Y_Abs - Object_temp->y))
                    {
												Num++;
												if (PT_max <= Object_Priority(Object_temp))
												{
                            PT_sec = PT_max;
                            PT_max = Object_Priority(Object_temp);
                            *ObjectSecond = *ObjectFirst;
                            *ObjectFirst = Object_temp;
                        }
												else if (PT_sec <= Object_Priority(Object_temp))
												{
                            PT_sec = Object_Priority(Object_temp);
                            *ObjectSecond = Object_temp;
												}
                    }
                }
            }
        }
    }

    return Num;
}
#endif

//Point the pointer to an picture memory of the same size as the object picture !!!PicOut WILL BE OVERWRITED
ObjectControlStatic_t xGenerateSameSizeEmptyPic(uint32_t **PicOut, const Object_t *ObjectSrc)
{
    if (ObjectSrc == NULL)
        return OBJECT_FAIL;

    *PicOut = (uint32_t *)calloc(ObjectSrc->Num_of_PicSize, sizeof(uint32_t)); //Generate a continue memory
    return OBJECT_SUCCESS;
}

//Set the Object to input x,y and layout it
ObjectControlStatic_t xSetObjectTo(Object_t *ObjectSrc, const int XAbs, const int YAbs)
{
#ifndef USE_OVERLAPPING
    if (ObjectSrc == NULL)
        return OBJECT_FAIL;

    int X_pre = ObjectSrc->x;
    int Y_pre = ObjectSrc->y;
    uint16_t X_tmp, Y_tmp, x, y, X_max, Y_max;

    Object_t *ObjectFirst, *ObjectSecond;
    uint8_t Overlapping_flg = 0;
    uint32_t X_OL, Y_OL, Width_OL, Length_OL, Color;
    if (xFindPicOverlapXYWL(&X_OL, &Y_OL, &Width_OL, &Length_OL,
                            ObjectSrc->Pic, ObjectSrc->Pic,
                            X_pre, Y_pre, ObjectSrc->Width, ObjectSrc->Length,
                            XAbs, YAbs, ObjectSrc->Width, ObjectSrc->Length) == OBJECT_SUCCESS)
    {
        Overlapping_flg = 1;
    }
    else
    {
        Overlapping_flg = 0;
    }

    int X_OL_Abs = ObjectSrc->x + (int)X_OL;
    int Y_OL_Abs = ObjectSrc->y + (int)Y_OL;
    int X_OL_Max_Abs = X_OL_Abs + (int)Width_OL;
    int Y_OL_Max_Abs = Y_OL_Abs + (int)Length_OL;

    ObjectSrc->x = XAbs;
    ObjectSrc->y = YAbs;
		
    //Is X_pre, Y_pre in the range of LCD
    if (vInLCDRange(&x, &y, &X_max, &Y_max, X_pre, Y_pre, ObjectSrc->Length, ObjectSrc->Width) == OBJECT_SUCCESS)
    {
        //Start	draw pixel at pre
        for (Y_tmp = y; Y_tmp < Y_max; Y_tmp++)
        {
            for (X_tmp = x; X_tmp < X_max; X_tmp++)
            {
                if (Overlapping_flg && ObjectSrc->Visible && (int)X_tmp >= X_OL_Abs && (int)X_tmp < X_OL_Max_Abs && (int)Y_tmp >= Y_OL_Abs && (int)Y_tmp < Y_OL_Max_Abs) //overlapping part
                {
                    continue;
                }

                uiFindObjectAtPixel(&ObjectFirst, &ObjectSecond, X_tmp, Y_tmp);
								if (ObjectFirst == NULL)
								{
										Color = GetBackgroundColor(X_tmp, Y_tmp);
								}
								else
								{
										Color = uiGetObjectPixel(ObjectFirst, (int)X_tmp - ObjectFirst->x, (int)Y_tmp - ObjectFirst->y);
								}
                ObjectLCDSetPixel(X_tmp, Y_tmp, Color);
            }
        }
    }

    if (!ObjectSrc->Visible)
    {
        return IS_Visible;
    }

    //Is X_now, Y_now in the range of LCD
    if (vInLCDRange(&x, &y, &X_max, &Y_max, XAbs, YAbs, ObjectSrc->Length, ObjectSrc->Width) == OBJECT_SUCCESS)
    {
        //Start	draw pixel at new
        for (Y_tmp = y; Y_tmp < Y_max; Y_tmp++)
        {
            for (X_tmp = x; X_tmp < X_max; X_tmp++)
            {
                uiFindObjectAtPixel(&ObjectFirst, &ObjectSecond, X_tmp, Y_tmp);
                if (ObjectFirst == NULL)
                {
                    if (Overlapping_flg && (int)X_tmp >= X_OL_Abs && (int)X_tmp < X_OL_Max_Abs && (int)Y_tmp >= Y_OL_Abs && (int)Y_tmp < Y_OL_Max_Abs) //overlapping part
                    {
                        Color = GetBackgroundColor(X_tmp, Y_tmp);
                        ObjectLCDSetPixel(X_tmp, Y_tmp, Color);
                    }
                }
                else
                {
                    Color = uiGetObjectPixel(ObjectFirst, (int)X_tmp - ObjectFirst->x, (int)Y_tmp - ObjectFirst->y);
                    ObjectLCDSetPixel(X_tmp, Y_tmp, Color);
                }
            }
        }
    }

    return OBJECT_SUCCESS;
}
#else
    if (ObjectSrc == NULL)
        return OBJECT_FAIL;

    //if (!ObjectSrc->Visible)
    //{
    //    ObjectSrc->x = XAbs;
    //    ObjectSrc->y = YAbs;
    //    return IS_Visible;
    //}

    int X_pre = ObjectSrc->x;
    int Y_pre = ObjectSrc->y;

    uint32_t Pixel_Src;
    uint32_t Pixel_OL_Src;
    uint32_t Pixel_Mov;
    uint32_t Pixel_OL_Mov;

    //Generate a overlapping picture
    uint32_t *OL_Pic_Src = NULL;
    uint32_t *OL_Pic_Mov = NULL;
    xGenerateSameSizeEmptyPic(&OL_Pic_Src, ObjectSrc);
    xGenerateSameSizeEmptyPic(&OL_Pic_Mov, ObjectSrc);
    xAllOverlapPartPic(&OL_Pic_Src, ObjectSrc, X_Y_NEGATIVE_DO_NOT_IGNORE);
    //
    //Set Object to new location
    ObjectSrc->x = XAbs;
    ObjectSrc->y = YAbs;
    xAllOverlapPartPic(&OL_Pic_Mov, ObjectSrc, X_Y_NEGATIVE_DO_NOT_IGNORE);
    //

    uint32_t X_OL, Y_OL, Width_OL, Length_OL;
    if (xFindPicOverlapXYWL(&X_OL, &Y_OL, &Width_OL, &Length_OL,
                            ObjectSrc->Pic, ObjectSrc->Pic,
                            X_pre, Y_pre, ObjectSrc->Width, ObjectSrc->Length,
                            XAbs, YAbs, ObjectSrc->Width, ObjectSrc->Length) == OBJECT_SUCCESS)
    {                                                                //Overlap before and after movement
        for (int y = Y_pre; y < Y_pre + (int)ObjectSrc->Length; y++) //at Pic_Src x,y ~ x+Pic_Src.x, y+Pic_Src.y
        {
            for (int x = X_pre; x < X_pre + (int)ObjectSrc->Width; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)LAYOUT_WIDTH && y < (int)LAYOUT_HIGHT)
                {
                    Pixel_Src = uiGetPicturePixel(ObjectSrc->Pic, ObjectSrc->Width, ObjectSrc->Length, x - X_pre, y - Y_pre);
                    Pixel_OL_Src = uiGetPicturePixel(OL_Pic_Src, ObjectSrc->Width, ObjectSrc->Length, x - X_pre, y - Y_pre);
										if(Pixel_Src & 0xFF000000)
										{
												 if(Pixel_Src == TEXT_COLOR)
													 Pixel_Src = Object_Color(ObjectSrc);
										}
										if(Pixel_OL_Src & 0xFF000000)
										{
												 if(Pixel_OL_Src == TEXT_COLOR)
													 Pixel_OL_Src = Object_Color(ObjectSrc);
										}
                    if (x < X_pre + (int)X_OL + (int)Width_OL && x >= (int)X_pre + (int)X_OL && y < Y_pre + (int)Y_OL + (int)Length_OL && y >= (int)Y_pre + (int)Y_OL) //The range that Src and Moved pics are overlapping
                    {
												
												if(X_pre == XAbs && Y_pre == YAbs)
												{
														if(ObjectSrc->Visible)
														{
																if(Pixel_Src != GetBackgroundColor(x, y))
																		ObjectLCDSetPixel(x, y, Pixel_Src);
														}
														else
														{
																ObjectLCDSetPixel(x, y, GetBackgroundColor(x, y));
														}
												}
												else
												{		
														Pixel_Mov = uiGetPicturePixel(ObjectSrc->Pic, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
														Pixel_OL_Mov = uiGetPicturePixel(OL_Pic_Mov, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
														if(Pixel_Mov & 0xFF000000)
														{
																 if(Pixel_Mov == TEXT_COLOR)
																	 Pixel_Mov = Object_Color(ObjectSrc);
														}
														if(Pixel_OL_Mov & 0xFF000000)
														{
																 if(Pixel_OL_Mov == TEXT_COLOR)
																	 Pixel_OL_Mov = Object_Color(ObjectSrc);
														}
														///0010
														if (Pixel_Src == GetBackgroundColor(x, y) && Pixel_OL_Mov == GetBackgroundColor(x, y) && Pixel_Mov != GetBackgroundColor(x, y))
														{
																ObjectLCDSetPixel(x, y, Pixel_Mov); //Write Mov Pixel
														}
														///0011
														///1010
														if (Pixel_Src != GetBackgroundColor(x, y) && Pixel_OL_Src == GetBackgroundColor(x, y) && Pixel_Mov != GetBackgroundColor(x, y))
														{
																if (Pixel_Src != Pixel_Mov)
																		ObjectLCDSetPixel(x, y, Pixel_Mov); //Write Mov Pixel
														}
														///1000
														if (Pixel_Src != GetBackgroundColor(x, y) && Pixel_OL_Src == GetBackgroundColor(x, y) && Pixel_Mov == GetBackgroundColor(x, y))
														{
																ObjectLCDSetPixel(x, y, GetBackgroundColor(x, y)); //Clear
														}
												}
                    }
                    else //!Pixel_Mov && !Pixel_OL_Mov
                    {
                        ///1000
                        if (Pixel_Src != GetBackgroundColor(x, y) && Pixel_OL_Src == GetBackgroundColor(x, y))
                        {
                            ObjectLCDSetPixel(x, y, GetBackgroundColor(x, y)); //Clear
                        }
                    }
                }
            }
        }
        for (int y = ObjectSrc->y; y < ObjectSrc->y + (int)ObjectSrc->Length; y++) //at Pic_Mov x,y ~ x+Pic_Mov.x, y+Pic_Mov.y
        {
            for (int x = ObjectSrc->x; x < ObjectSrc->x + (int)ObjectSrc->Width; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)LAYOUT_WIDTH && y < (int)LAYOUT_HIGHT)
                {
                    if (!(x < X_pre + (int)X_OL + (int)Width_OL && x >= X_pre + (int)X_OL && y < Y_pre + (int)Y_OL + (int)Length_OL && y >= Y_pre + (int)Y_OL)) //The range that Src and Moved pics are not overlapping
                    {
                        Pixel_Mov = uiGetPicturePixel(ObjectSrc->Pic, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
                        Pixel_OL_Mov = uiGetPicturePixel(OL_Pic_Mov, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
												if(Pixel_Mov & 0xFF000000)
												{
														 if(Pixel_Mov == TEXT_COLOR)
															 Pixel_Mov = Object_Color(ObjectSrc);
												}
												if(Pixel_OL_Mov & 0xFF000000)
												{
														 if(Pixel_OL_Mov == TEXT_COLOR)
															 Pixel_OL_Mov = Object_Color(ObjectSrc);
												}
                        ///
                        if (Pixel_Mov != GetBackgroundColor(x, y) && Pixel_OL_Mov == GetBackgroundColor(x, y))
                        {
                            ObjectLCDSetPixel(x, y, Pixel_Mov);
                        }
                        ///
                    }
                }
            }
        }
    }
    else //No overlap before and after movement
    {
        for (int y = Y_pre; y < Y_pre + (int)ObjectSrc->Length; y++) //at Pic_Src x,y ~ x+Pic_Src.x, y+Pic_Src.y
        {
            for (int x = X_pre; x < X_pre + (int)ObjectSrc->Width; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)LAYOUT_WIDTH && y < (int)LAYOUT_HIGHT)
                {
                    Pixel_Src = uiGetPicturePixel(ObjectSrc->Pic, ObjectSrc->Width, ObjectSrc->Length, x - X_pre, y - Y_pre);
                    Pixel_OL_Src = uiGetPicturePixel(OL_Pic_Src, ObjectSrc->Width, ObjectSrc->Length, x - X_pre, y - Y_pre);
										if(Pixel_Src & 0xFF000000)
										{
												 if(Pixel_Src == TEXT_COLOR)
													 Pixel_Src = Object_Color(ObjectSrc);
										}
										if(Pixel_OL_Src & 0xFF000000)
										{
												 if(Pixel_OL_Src == TEXT_COLOR)
													 Pixel_OL_Src = Object_Color(ObjectSrc);
										}
                    if (Pixel_Src != GetBackgroundColor(x, y) && Pixel_OL_Src == GetBackgroundColor(x, y))
                    {
                        ObjectLCDSetPixel(x, y, GetBackgroundColor(x, y)); //Clear
                    }
                }
            }
        }
        for (int y = ObjectSrc->y; y < ObjectSrc->y + (int)ObjectSrc->Length; y++) //at Pic_Mov x,y ~ x+Pic_Mov.x, y+Pic_Mov.y
        {
            for (int x = ObjectSrc->x; x < ObjectSrc->x + (int)ObjectSrc->Width; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)LAYOUT_WIDTH && y < (int)LAYOUT_HIGHT)
                {
                    Pixel_Mov = uiGetPicturePixel(ObjectSrc->Pic, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
                    Pixel_OL_Mov = uiGetPicturePixel(OL_Pic_Mov, ObjectSrc->Width, ObjectSrc->Length, x - ObjectSrc->x, y - ObjectSrc->y);
										if(Pixel_Mov & 0xFF000000)
										{
												 if(Pixel_Mov == TEXT_COLOR)
													 Pixel_Mov = Object_Color(ObjectSrc);
										}
										if(Pixel_OL_Mov & 0xFF000000)
										{
												 if(Pixel_OL_Mov == TEXT_COLOR)
													 Pixel_OL_Mov = Object_Color(ObjectSrc);
										}
                    ///
                    if (Pixel_Mov != GetBackgroundColor(x, y) && Pixel_OL_Mov == GetBackgroundColor(x, y))
                    {
                        ObjectLCDSetPixel(x, y, Pixel_Mov);
                    }
                    ///
                }
            }
        }
    }

    //Write and Clear pixels
    free(OL_Pic_Src);
    free(OL_Pic_Mov);

    return OBJECT_SUCCESS;
}
#endif
//Move the Object and layout it
ObjectControlStatic_t xMoveObject(Object_t *ObjectSrc, const int XAdd, const int YAdd)
{
    return xSetObjectTo(ObjectSrc, ObjectSrc->x + XAdd, ObjectSrc->y + YAdd);
}

//Output all picture overlapping with ObjectSrc part. Can set ignore x, y>0  !!!Pic and ObjectSre->Pic SIZE MUST be the same(xGenerateSameSizeEmptyPic) !!!Pic WILL BE OVERWRITED
ObjectControlStatic_t xAllOverlapPartPic(uint32_t **Pic, const Object_t *ObjectSrc, const IsXYNegativeIgnore_t isIgnore)
{
    if (ObjectSrc == NULL)
        return OBJECT_FAIL;
    Object_t *ObjectCmp = NULL;

    for (uint32_t Object_ptr = 0; Object_ptr < MAX_OBJECT; Object_ptr++)
    {
        if (xGetObject(&ObjectCmp, Object_ptr) == OBJECT_SUCCESS)
        {
            if (ObjectCmp != ObjectSrc && ObjectCmp->Visible)
            {
                xOverlapPartPic(Pic, ObjectSrc, ObjectCmp, isIgnore);
            }
        }
    }
    return OBJECT_SUCCESS;
}

//Output 2 picture overlapping part. Can set ignore x, y>0  !!!Pic and ObjectSre->Pic SIZE MUST be the same !!!Pic WILL BE OVERWRITED
ObjectControlStatic_t xOverlapPartPic(uint32_t **Pic, const Object_t *ObjectSrc, const Object_t *ObjectCmp, const IsXYNegativeIgnore_t isIgnore)
{
    if (ObjectSrc == NULL || ObjectCmp == NULL)
        return OBJECT_FAIL;

    uint32_t XRel, YRel, Width, Length;
    ObjectControlStatic_t State;

    State = xFindObjectOverlapXYWL(&XRel, &YRel, &Width, &Length, ObjectSrc, ObjectCmp);
    if (State == OBJECT_SUCCESS)
    {
        switch (isIgnore)
        {
        case X_Y_NEGATIVE_IGNORE: //There is values at Pic only if XAbs>=0 YAbs>=0
            for (uint32_t y = 0; y < Length; y++)
            {
                for (uint32_t x = 0; x < Width; x++)
                {
                    if (ObjectSrc->x + (int)(XRel + x) >= 0 && ObjectSrc->y + (int)(YRel + y) >= 0)
                    {
#ifdef TP
                        if (!Tp(ObjectSrc, XRel + x, YRel + y) && !Tp(ObjectCmp, (ObjectSrc->x + x + XRel) - ObjectCmp->x, (ObjectSrc->y + y + YRel) - ObjectCmp->y))
                        { //Tp = 0
#endif
                            if ((uiGetObjectPixel(ObjectSrc, XRel + x, YRel + y) != BACKGROUND_COLOR) && (uiGetObjectPixel(ObjectCmp, (ObjectSrc->x + x + XRel) - ObjectCmp->x, (ObjectSrc->y + y + YRel) - ObjectCmp->y) != BACKGROUND_COLOR)) //Overlapping pixel
                            {
                                vSetPicturePixel(*Pic, ObjectSrc->Width, ObjectSrc->Length, x + XRel, y + YRel, 1);
                            }
#ifdef TP
                        }
#endif
                    }
                }
            }
            break;
        case X_Y_NEGATIVE_DO_NOT_IGNORE:
            for (uint32_t y = 0; y < Length; y++)
            {
                for (uint32_t x = 0; x < Width; x++)
                {
#ifdef TP
                    if (!Tp(ObjectSrc, XRel + x, YRel + y) && !Tp(ObjectCmp, (ObjectSrc->x + x + XRel) - ObjectCmp->x, (ObjectSrc->y + y + YRel) - ObjectCmp->y))
                    { //Tp = 0
#endif
                        if ((uiGetObjectPixel(ObjectSrc, XRel + x, YRel + y) != BACKGROUND_COLOR) && (uiGetObjectPixel(ObjectCmp, (ObjectSrc->x + x + XRel) - ObjectCmp->x, (ObjectSrc->y + y + YRel) - ObjectCmp->y) != BACKGROUND_COLOR)) //Overlapping pixel
                        {
                            vSetPicturePixel(*Pic, ObjectSrc->Width, ObjectSrc->Length, x + XRel, y + YRel, 1);
                        }
#ifdef TP
                    }
#endif
                }
            }
            break;
        }
        return OBJECT_SUCCESS;
    }
    else if (State == OBJECT_FAIL)
    {
        return OBJECT_FAIL;
    }
    return OBJECT_SUCCESS;
}

//Set Object as a full circle. Circle x,y at left, top  !!!Object DATA WILL BE OVERWRITED
/*
ObjectControlStatic_t xObjectType_ImportPic(Object_t *Object, const uint32_t Radius)
{
#ifndef USE_ARGB_PIC
    if (Object->Pic != NULL)
    {
        free(Object->Pic);
    }

    DrawCircleToPicture(&(Object->Pic), &(Object->Length), &(Object->Width), Radius);
    Object->Num_of_PicSize = (Object->Length) * (Object->Width);
    if ((Object->Num_of_PicSize & 0xf) == 0) //Divisible by 32
    {
        Object->Num_of_PicSize = (Object->Num_of_PicSize >> 5);
    }
    else
    {
        Object->Num_of_PicSize = (Object->Num_of_PicSize >> 5) + 1;
    }
    return OBJECT_SUCCESS;
#else
    if (Object->Pic != NULL)
    {
        free(Object->Pic);
    }

    DrawCircleToPicture(&(Object->Pic), &(Object->Length), &(Object->Width), Radius);
    Object->Num_of_PicSize = (Object->Length) * (Object->Width);
    return OBJECT_SUCCESS;
#endif
}
*/
ObjectControlStatic_t xObjectType_ImportPic(Object_t *Object, ObjectPicture_t *Picture)
{
#ifndef USE_ARGB_PIC
    Object->Pic = Picture->Pic;
    Object->Width = Picture->Width;
    Object->Length = Picture->Length;

    Object->Num_of_PicSize = (Object->Length) * (Object->Width);
    if ((Object->Num_of_PicSize & 0xf) == 0) //Divisible by 32
    {
        Object->Num_of_PicSize = (Object->Num_of_PicSize >> 5);
    }
    else
    {
        Object->Num_of_PicSize = (Object->Num_of_PicSize >> 5) + 1;
    }
    return OBJECT_SUCCESS;
#else
    Object->Pic = Picture->Pic;
    Object->Width = Picture->Width;
    Object->Length = Picture->Length;
		
    Object->Num_of_PicSize = (Object->Length) * (Object->Width);
    return OBJECT_SUCCESS;
#endif
}

//Generate a continuous memory picture(*Pic) containing a circle
/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//           x 0 1 2 - - - W-1        W = Width       Pic+n X is don't care        //
//        y   ---------------→        L = Length                                   //
//        0  | 0 1 2 - - - W-1                                                     //
//        1  |            2W-1                                                     //
// Pic => 2  |            3W-1     *Pic     (bit)={ 0, 1, 2,..., sizeof(uint32_t)} //
//        -  |              -      *(Pic+1) (bit)={32,33,34,...,2sizeof(uint32_t)} //
//        -  |              -          -                                           //
//       L-1 ↓            LW-1     *(Pic+n) (bit)={LW-3,LW-2,LW-1, X, X, X,...}    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
ObjectControlStatic_t DrawCircleToPicture(uint32_t **Pic, uint32_t *Length, uint32_t *Width, const uint32_t Radius) //x,y at left, top
{
#ifndef USE_ARGB_PIC
    uint32_t PicSize = 0, PicBit_ptr = 0, IntSize = 0, PicTempBit = 0;
    uint32_t *PicTemp = NULL;
    free(*Pic);

    *Width = 0;
    *Length = 2 * Radius + 1;
#ifndef DEBUG
    if (Radius > 15)
        return OBJECT_FAIL;
#else
    if (Radius > 15)
        return ERROR;
#endif

    for (int i = 0; i < 2; i++)
    {
        int decision;       /* Decision Variable */
        uint32_t current_x; /* Current X Value */
        uint32_t current_y; /* Current Y Value */
        if (i == 1)
        {
            PicSize = (*Length) * (*Width);
            if ((PicSize & 0xf) == 0) //Divisible by 32
            {
                PicSize = (PicSize >> 5);
            }
            else
            {
                PicSize = (PicSize >> 5) + 1;
            }
            *Pic = (uint32_t *)calloc(PicSize, sizeof(uint32_t)); //Generate a continue memory
            PicTemp = (uint32_t *)calloc(*Length, sizeof(uint32_t));
        }

        decision = 3 - (Radius << 1);

        current_x = 0;
        current_y = Radius;

        while (current_x <= current_y)
        {
            if (current_y > 0)
            {
                if (i == 1)
                {
                    for (int j = 0; j < 2 * current_y; j++)
                    {
                        *(PicTemp + (current_x + Radius)) |= 1 << (31 - (-current_y + Radius + j));
                        *(PicTemp + (-current_x + Radius)) |= 1 << (31 - (-current_y + Radius + j));
                    }
                }
                else
                {
                    if (*Width < 2 * current_y)
                        *Width = 2 * current_y;
                }
            }

            if (current_x > 0)
            {
                if (i == 1)
                {
                    for (int j = 0; j < 2 * current_x; j++)
                    {
                        *(PicTemp + (-current_y + Radius)) |= 1 << (31 - (-current_x + Radius + j));
                        *(PicTemp + (current_y + Radius)) |= 1 << (31 - (-current_x + Radius + j));
                    }
                }
                else
                {
                    if (*Width < 2 * current_x)
                        *Width = 2 * current_x;
                }
            }

            if (decision < 0)
            {
                decision += (current_x << 2) + 6;
            }
            else
            {
                decision += ((current_x - current_y) << 2) + 10;
                current_y--;
            }
            current_x++;
        }
    }

    for (int y = 0; y < *Length; y++)
    {
        for (int x = 0; x < *Width; x++)
        {
            PicTempBit = ((*(PicTemp + y) >> (31 - x)) & 1);
            *(*Pic + PicBit_ptr) = (*(*Pic + PicBit_ptr) << 1) | PicTempBit;
            if (IntSize++ == 31)
            {
                PicBit_ptr++;
                IntSize = 0;
            }
        }
    }
    for (; IntSize < 32; IntSize++)
    {
        *(*Pic + PicBit_ptr) <<= 1;
    }

    free(PicTemp);

    return OBJECT_SUCCESS;
#else
    uint32_t PicSize = 0, PicBit_ptr = 0, PicTempBit = 0;
    uint32_t *PicTemp = NULL;
    free(*Pic);

    *Width = 0;
    *Length = 2 * Radius + 1;
#ifndef DEBUG
    if (Radius > 15)
        return OBJECT_FAIL;
#else
    if (Radius > 15)
        return ERROR;
#endif
    for (int i = 0; i < 2; i++)
    {
        int decision;       /* Decision Variable */
        uint32_t current_x; /* Current X Value */
        uint32_t current_y; /* Current Y Value */
        if (i == 1)
        {
            PicSize = (*Length) * (*Width);
            *Pic = (uint32_t *)calloc(PicSize, sizeof(uint32_t)); //Generate a continue memory
						if(*Pic==NULL)
						{
								printf("Memory not enough!!");
								for(;;);
						}
            PicTemp = (uint32_t *)calloc(*Length, sizeof(uint32_t));
        }

        decision = 3 - (Radius << 1);

        current_x = 0;
        current_y = Radius;

        while (current_x <= current_y)
        {
            if (current_y > 0)
            {
                if (i == 1)
                {
                    for (int j = 0; j < 2 * current_y; j++)
                    {
                        *(PicTemp + (current_x + Radius)) |= 1 << (31 - (-current_y + Radius + j));
                        *(PicTemp + (-current_x + Radius)) |= 1 << (31 - (-current_y + Radius + j));
                    }
                }
                else
                {
                    if (*Width < 2 * current_y)
                        *Width = 2 * current_y;
                }
            }

            if (current_x > 0)
            {
                if (i == 1)
                {
                    for (int j = 0; j < 2 * current_x; j++)
                    {
                        *(PicTemp + (-current_y + Radius)) |= 1 << (31 - (-current_x + Radius + j));
                        *(PicTemp + (current_y + Radius)) |= 1 << (31 - (-current_x + Radius + j));
                    }
                }
                else
                {
                    if (*Width < 2 * current_x)
                        *Width = 2 * current_x;
                }
            }

            if (decision < 0)
            {
                decision += (current_x << 2) + 6;
            }
            else
            {
                decision += ((current_x - current_y) << 2) + 10;
                current_y--;
            }
            current_x++;
        }
    }

		uint32_t Color;
    for (int y = 0; y < *Length; y++)
    {
        for (int x = 0; x < *Width; x++)
        {
						Color = TEXT_COLOR;
								
						uint32_t NowColor_A;
						if(x > y)
						{
								NowColor_A = ((uint32_t)((float)x / (float)*Width * (float)0x80) << (32-8));
						}
						else
						{
								NowColor_A = ((uint32_t)((float)y / (float)*Length * (float)0x80) << (32-8));
						}
						Color &= 0x00FFFFFF;
						Color |= 0xFF000000 - NowColor_A;
						
            PicTempBit = ((*(PicTemp + y) >> (31 - x)) & 1);
						if(PicTempBit)
							*(*Pic + PicBit_ptr) = Color;
						else
							*(*Pic + PicBit_ptr) = TRANSPARANT_COLOR;
            
            PicBit_ptr++;
        }
    }

    free(PicTemp);

    return OBJECT_SUCCESS;
	#endif
}

//If Pics are overlapping, return OBJECT_SUCCESS
ObjectControlStatic_t xIsPicOverlapping(const uint32_t *PicSrc, const uint32_t *PicCmp, const int X_Src, const int Y_Src, const uint32_t Width_Src, const uint32_t Length_Src, const int X_Cmp, const int Y_Cmp, const uint32_t Width_Cmp, const uint32_t Length_Cmp)
{
    if (X_Cmp >= X_Src && X_Cmp < X_Src + (int)Width_Src) //ObjectCmp is to the right of ObjectSrc
    {
    }
    else if (X_Cmp <= X_Src && X_Cmp + (int)Width_Cmp > X_Src) //ObjectCmp is to the left of ObjectSrc
    {
    }
    else
    {
        return OBJECT_FAIL;
    }
    if (Y_Cmp >= Y_Src && Y_Cmp < Y_Src + (int)Length_Src) //ObjectCmp is below ObjectSrc
    {
    }
    else if (Y_Cmp <= Y_Src && Y_Cmp + (int)Length_Cmp > Y_Src) //ObjectCmp is above ObjectSrc
    {
    }
    else
    {
        return OBJECT_FAIL;
    }
    return OBJECT_SUCCESS;
}

//If Pics are overlapping(not contain transparent parts), return OBJECT_SUCCESS
ObjectControlStatic_t xIsPicOverlappingTP(const uint32_t *PicSrc, const uint32_t *PicCmp, const int X_Src, const int Y_Src, const uint32_t Width_Src, const uint32_t Length_Src, const int X_Cmp, const int Y_Cmp, const uint32_t Width_Cmp, const uint32_t Length_Cmp)
{
    uint32_t XRel, YRel, Width, Length;
    ObjectControlStatic_t State;

    State = xFindPicOverlapXYWL(&XRel, &YRel, &Width, &Length, PicSrc, PicCmp, X_Src, Y_Src, Width_Src, Length_Src, X_Cmp, Y_Cmp, Width_Cmp, Length_Cmp);
    if (State == OBJECT_SUCCESS)
    {
        for (uint32_t y = 0; y < Length; y++)
        {
            for (uint32_t x = 0; x < Width; x++)
            {
                if (X_Src + (int)(XRel + x) >= 0 && Y_Src + (int)(YRel + y) >= 0)
                {
                    if ((uiGetPicturePixel(PicSrc, Width_Src, Length_Src, XRel + x, YRel + y) != BACKGROUND_COLOR) && (uiGetPicturePixel(PicCmp, Width_Cmp, Length_Cmp, (X_Src + x + XRel) - X_Cmp, (Y_Src + y + YRel) - Y_Cmp) != BACKGROUND_COLOR)) //Overlapping pixel
                    {
                        return OBJECT_TRUE;
                    }
                }
            }
        }
        return OBJECT_FALSE;
    }
    return OBJECT_FAIL;
}

//If Objects are overlapping, return OBJECT_SUCCESS
ObjectControlStatic_t xIsObjectOverlapping(const Object_t *ObjectSrc, const Object_t *ObjectCmp)
{
    if (ObjectSrc != NULL && ObjectCmp != NULL)
    {
        return xIsPicOverlapping(ObjectSrc->Pic, ObjectCmp->Pic,
                                 ObjectSrc->x, ObjectSrc->y, ObjectSrc->Width, ObjectSrc->Length,
                                 ObjectCmp->x, ObjectCmp->y, ObjectCmp->Width, ObjectCmp->Length);
    }
    else
    {
        return OBJECT_FAIL;
    }
}

//If Objects are overlapping(not contain transparent parts), return OBJECT_SUCCESS
ObjectControlStatic_t xIsObjectOverlappingTP(const Object_t *ObjectSrc, const Object_t *ObjectCmp)
{
    if (ObjectSrc != NULL && ObjectCmp != NULL)
    {
        return xIsPicOverlappingTP(ObjectSrc->Pic, ObjectCmp->Pic,
                                   ObjectSrc->x, ObjectSrc->y, ObjectSrc->Width, ObjectSrc->Length,
                                   ObjectCmp->x, ObjectCmp->y, ObjectCmp->Width, ObjectCmp->Length);
    }
    else
    {
        return OBJECT_FAIL;
    }
}

//Get the length and width of the overlapping part of the 2 pictures and the upper left point
ObjectControlStatic_t xFindPicOverlapXYWL(uint32_t *XRel, uint32_t *YRel, uint32_t *Width, uint32_t *Length, const uint32_t *Pic_Src, const uint32_t *Pic_Cmp, const int X_Src, const int Y_Src, const uint32_t Width_Src, const uint32_t Length_Src, const int X_Cmp, const int Y_Cmp, const uint32_t Width_Cmp, const uint32_t Length_Cmp)
{
    ObjectControlStatic_t State;

    State = xIsPicOverlapping(Pic_Src, Pic_Cmp, X_Src, Y_Src, Width_Src, Length_Src, X_Cmp, Y_Cmp, Width_Cmp, Length_Cmp);
    if (State == OBJECT_FAIL)
    {
        return OBJECT_FAIL;
    }
    else
    {
        if (X_Src > X_Cmp) //ObjectCmp.x<ObjectSrc.x
        {
            *XRel = 0;
            if (X_Src + Width_Src >= X_Cmp + Width_Cmp)
            {
                *Width = (X_Cmp + Width_Cmp) - X_Src;
            }
            else
            {
                *Width = Width_Src;
            }
        }
        else if (X_Src <= X_Cmp) //ObjectCmp.x>=ObjectSrc.x
        {
            *XRel = X_Cmp - X_Src;
            if (X_Src + Width_Src >= X_Cmp + Width_Cmp)
            {
                *Width = Width_Cmp;
            }
            else
            {
                *Width = Width_Src - *XRel;
            }
        }
        if (Y_Src > Y_Cmp) //ObjectCmp.y<ObjectSrc.y
        {
            *YRel = 0;
            if (Y_Src + Length_Src >= Y_Cmp + Length_Cmp)
            {
                *Length = (Y_Cmp + Length_Cmp) - Y_Src;
            }
            else
            {
                *Length = Length_Src;
            }
        }
        else if (Y_Src <= Y_Cmp) //ObjectCmp.y>=ObjectSrc.y
        {
            *YRel = Y_Cmp - Y_Src;
            if (Y_Src + Length_Src >= Y_Cmp + Length_Cmp)
            {
                *Length = Length_Cmp;
            }
            else
            {
                *Length = Length_Src - *YRel;
            }
        }
        return OBJECT_SUCCESS;
    }
}

//Get the length and width of the overlapping part of the 2 object pictures and the upper left point
ObjectControlStatic_t xFindObjectOverlapXYWL(uint32_t *XRel, uint32_t *YRel, uint32_t *Width, uint32_t *Length, const Object_t *ObjectSrc, const Object_t *ObjectCmp)
{

    if (ObjectSrc == NULL || ObjectCmp == NULL)
        return OBJECT_FAIL;
    else
    {
        return xFindPicOverlapXYWL(XRel, YRel, Width, Length, ObjectSrc->Pic, ObjectCmp->Pic, ObjectSrc->x, ObjectSrc->y, ObjectSrc->Width, ObjectSrc->Length, ObjectCmp->x, ObjectCmp->y, ObjectCmp->Width, ObjectCmp->Length);
    }
}

//Initialize Object library
ObjectControlStatic_t xInitObject(void)
{
    return xObjectData(NULL, INITIAL, 0);
}

//Create a new Object and store it into object database   !!!MUST POINT Object TO NULL IF IT IS NEW Object
ObjectControlStatic_t xNewObject(Object_t **Object)
{
    return xObjectData(Object, NEW, 0);
}

//Get object in the database[ptr] by using number(ptr)
ObjectControlStatic_t xGetObject(Object_t **Object, uint32_t ptr)
{
    return xObjectData(Object, GET, ptr);
}

//Get object in the database[ptr] by using Mark(char*=>uint32_t)
ObjectControlStatic_t xGetObjectByMark(Object_t **Object, Mark_t Mark)
{
    return xObjectData(Object, GET_BY_MARK, (uint32_t)Mark);
}

//Fully delete Object(Including database and Object pointer)
ObjectControlStatic_t xDeleteObject(Object_t **Object)
{
#ifdef OBJECT_GROUP_DEFINED
    xObjectGroup_RemoveObjFromAllGroup(*Object);
#endif
    return xObjectData(Object, DEL, 0);
}

//Functions which controls all objects. All object pointer are in here  !!!MUST POINT Object TO NULL IF IT IS NEW Object
ObjectControlStatic_t xObjectData(Object_t **Object, const ObjectControl_t Control, uint32_t ptr)
{
    static Object_t *ObjectList[MAX_OBJECT];
    static bool ObjectEmpty[MAX_OBJECT];

    switch (Control)
    {
    case INITIAL:
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            free(ObjectList[ptr]);
            //ObjectList[ptr] = NULL;
            ObjectEmpty[ptr] = true;
        }
        return OBJECT_SUCCESS;
#ifdef DEBUG
        break;
#endif
    case NEW:
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            if (ObjectEmpty[ptr])
            {
                Object_t *NewObject = NULL;
                if (*Object != NULL)
                    free(*Object);
                NewObject = (Object_t *)calloc(1, sizeof(Object_t));
                if (NewObject == NULL)
                {
                    return NO_MEMORY_SPACE;
                }
                *Object = NewObject;
                ObjectEmpty[ptr] = false;
                ObjectList[ptr] = *Object;
                return OBJECT_SUCCESS;
            }
            else
            {
                if (ptr == MAX_OBJECT - 1)
                    return OBJECT_FAIL;
            }
        }
#ifdef DEBUG
        break;
#endif
    case DEL:
        for (ptr = 0; ptr < MAX_OBJECT; ptr++)
        {
            if (ObjectList[ptr] == *Object)
            {
                xSetVisible(*Object, 0);
                free(*Object);
                *Object = NULL;
                ObjectList[ptr] = NULL;
                ObjectEmpty[ptr] = true;
                return OBJECT_SUCCESS;
            }
        }
        return OBJECT_FAIL;
#ifdef DEBUG
        break;
#endif
    case GET:
        if (ObjectEmpty[ptr])
        {
            return OBJECT_FAIL;
        }
        else
        {
            *Object = ObjectList[ptr];
            return OBJECT_SUCCESS;
        }
#ifdef DEBUG
        break;
#endif
    case GET_BY_MARK:
        for (uint32_t ptr_temp = 0; ptr_temp < MAX_OBJECT; ptr_temp++)
        {
            if (!ObjectEmpty[ptr_temp])
            {
                if ((ObjectList[ptr_temp]->Mark) == (Mark_t)ptr) //Find
                {
                    *Object = ObjectList[ptr_temp];
                    return OBJECT_SUCCESS;
                }
            }
        }
        return OBJECT_FAIL;
#ifdef DEBUG
        break;
#endif
    default:
        return CONTROL_STATE_WRONG;
#ifdef DEBUG
        break;
#endif
    }
}

static ObjectControlStatic_t vInLCDRange(uint16_t *X_Abs, uint16_t *Y_Abs, uint16_t *X_max, uint16_t *Y_max, const int x, const int y, const int Length, const int Width)
{
    if (x >= 0 && x < (int)LAYOUT_WIDTH)
    {
        *X_Abs = x;
        if (x + Width < (int)LAYOUT_WIDTH)
        {
            *X_max = x + Width;
        }
        else
        {
            *X_max = LAYOUT_WIDTH;
        }
    }
    else if (x < 0 && x + Width > 0)
    {
        *X_max = x + Width;
        *X_Abs = 0;
    }
    else
    {
        //Not in LCD Range
        return OBJECT_FAIL;
    }

    if (y >= 0 && y < (int)LAYOUT_HIGHT)
    {
        *Y_Abs = y;
        if (y + Length < (int)LAYOUT_HIGHT)
        {
            *Y_max = y + Length;
        }
        else
        {
            *Y_max = LAYOUT_HIGHT;
        }
    }
    else if (y < 0 && y + Length > 0)
    {
        *Y_max = y + Length;
        *Y_Abs = 0;
    }
    else
    {
        //Not in LCD Range
        return OBJECT_FAIL;
    }
    return OBJECT_SUCCESS;
}

static uint8_t ucIsPixelTP(Object_t *Object, uint16_t X_Rel, uint16_t Y_Rel)
{
	#ifndef USE_ARGB_PIC
    uint32_t Color = uiGetObjectPixel(Object, X_Rel, Y_Rel);
    //if((Color & 0x11000000) == 0)
    if (Color == BACKGROUND_COLOR)
    {
        return 1;
    }
    else
    {
        return 0;
    }
	#else
    uint32_t Color = uiGetObjectPixel(Object, X_Rel, Y_Rel);
    //if((Color & 0x11000000) == 0)
    if (Color & 0xFF000000)
    {
        return 0;
    }
    else
    {
        return 1;
    }
	#endif
}

//Is object the touch left or right bound
ObjectFindTop_t ucIsTouchBound_LR(const Object_t *Object)
{
		if(Object_x(Object) <= LEFT_BOUND)
			return AT_LEFT;
		else if(Object_xMax(Object) >= RIGHT_BOUND - 1)
			return AT_RIGHT;
		else
			return NOT_FOUND;
}

//Is object the touch top or bottom bound
ObjectFindTop_t ucIsTouchBound_TB(const Object_t *Object)
{
		if(Object_y(Object) <= TOP_BOUND)
			return AT_TOP;
		else if(Object_yMax(Object) >= BOTTOM_BOUND - 1)
			return AT_BOTTOM;
		else
			return NOT_FOUND;
}

