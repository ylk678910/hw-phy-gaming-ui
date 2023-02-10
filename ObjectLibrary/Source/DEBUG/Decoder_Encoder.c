#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
void Encoder(uint32_t **output, const uint32_t MODE);
void uiGetPicturePixel(uint32_t *data, uint32_t *Tp, const uint32_t *Pic, const uint32_t RelX, const uint32_t RelY);
void Decoder(const uint32_t *output);

#define XMAX 7
#define YMAX 5

int main()
{
    uint32_t *output = NULL;

    Encoder(&output, 1);
    Decoder(output);
    system("PAUSE");
    return 0;
}

void uiGetPicturePixel(uint32_t *data, uint32_t *Tp, const uint32_t *Pic, const uint32_t RelX, const uint32_t RelY)
{
    uint32_t mode = *Pic >> 30;
    uint32_t Width = (*Pic & 0x3fffffff) >> 21;
    uint32_t Length = (*Pic & 0x001FFFFF) >> 12;
    uint32_t PixelLocation;
    uint32_t PicData;
    uint32_t temp;

    switch (mode)
    {
    case 0:
        PixelLocation = Width * RelY + RelX + 32;
        PicData = *(Pic + (PixelLocation >> 5));
        *data = (PicData & (1 << (31 - (PixelLocation & 31)))) > 0;
        *Tp = 0;
        break;
    case 1:
        PixelLocation = Width * RelY + RelX + 33; //N
        temp = (PixelLocation << 1) + 33;         //N*2+M
        PicData = *(Pic + (temp >> 5));           //(N*2+M)/sizeof(a)
        *Tp = (PicData & (1 << (31 - (temp & 31)))) > 0;
        if ((temp & 31) == 31)
        {
            *data = (*(Pic + (temp >> 5) + 1) & (1 << 31)) > 0;
        }
        else
        {
            *data = (PicData & (1 << (31 - ((temp & 31) + 1)))) > 0;
        }

        break;
    case 2:
        PixelLocation = Width * RelY + RelX + 37;
        PicData = *(Pic + (PixelLocation >> 5));
        *data = (PicData & (1 << (31 - (PixelLocation & 31)))) > 0;
        *Tp = 0;
        break;
    case 3:
        PixelLocation = Width * RelY + RelX + 37;
        PicData = *(Pic + (PixelLocation >> 5));
        *data = (PicData & (1 << (31 - (PixelLocation & 31)))) > 0;
        //*Tp = ;
        break;

    default:
        break;
    }
}

void Decoder(const uint32_t *output)
{
    uint32_t mode = *output >> 30;
    uint32_t Width = (*output & 0x3fffffff) >> 21;
    uint32_t Length = (*output & 0x001FFFFF) >> 12;
    uint32_t PtrSize, Data, Tp;
    if (mode & 2)
    {
        printf("Color\n");
    }
    else
    {
        printf("noColor\n");
    }
    if (mode & 1)
    {
        printf("Tp\n");
    }
    else
    {
        printf("noTp\n");
    }
    printf("Width=%d\n", Width);
    printf("Length=%d\n", Length);

    switch (mode)
    {
    case 0:
        PtrSize = (*output & 0xfff);
        printf("PtrSize=%d\n", PtrSize);

        for (int y = 0; y < Length; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                uiGetPicturePixel(&Data, &Tp, output, x, y);
                printf("%x ", Data);
            }
            printf("\n");
        }
        break;
    case 1:
        PtrSize = ((*output & 0xfff) << 1) | ((*(output + 1) & 0x1 << 31) > 0);
        printf("PtrSize=%d\n", PtrSize);

        for (int y = 0; y < Length; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                uiGetPicturePixel(&Data, &Tp, output, x, y);
                printf("%x ", Data);
            }
            printf("\n");
        }
        printf("\n\n");
        for (int y = 0; y < Length; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                uiGetPicturePixel(&Data, &Tp, output, x, y);
                printf("%x ", Tp);
            }
            printf("\n");
        }
        break;
    case 2:
        PtrSize = (*output & 0xfff);
        printf("PtrSize=%d\n", PtrSize);

        for (int y = 0; y < Length; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                uiGetPicturePixel(&Data, &Tp, output, x, y);
                printf("%x ", Data);
            }
            printf("\n");
        }
        break;
    case 3:
        PtrSize = (*output & 0xfff);
        printf("PtrSize=%d\n", PtrSize);

        for (int y = 0; y < Length; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                uiGetPicturePixel(&Data, &Tp, output, x, y);
                printf("%x ", Data);
            }
            printf("\n");
        }
        break;

    default:
        break;
    }
}

void Encoder(uint32_t **output, const uint32_t MODE)
{
    uint32_t Width = XMAX, Length = YMAX;
    //

    uint32_t data0[YMAX][XMAX] = {{1, 0, 1, 1, 0, 0, 1},
                                  {0, 0, 0, 0, 1, 1, 0},
                                  {0, 0, 1, 0, 1, 1, 1},
                                  {1, 1, 1, 1, 0, 1, 1},
                                  {0, 0, 0, 0, 1, 0, 0}};

    uint32_t data1[YMAX][XMAX] = {{32, 0, 26, 12, 0, 0, 6},
                                  {0, 0, 0, 0, 34, 11, 0},
                                  {0, 0, 4642, 0, 1, 88, 5},
                                  {74, 152, 7452, 45, 0, 6, 235},
                                  {0, 0, 0, 0, 34, 0, 0}};

    uint32_t tp[YMAX][XMAX] = {{1, 0, 1, 1, 1, 1, 1},
                               {1, 0, 0, 0, 1, 1, 1},
                               {0, 1, 1, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 0, 0},
                               {0, 1, 1, 0, 0, 0, 0}};

    //fuction start
    uint32_t PtrSize, Uint_ptr = 0, ptr = 0;
    switch (MODE)
    {
    case 0:
        PtrSize = (XMAX) * (YMAX) + 2 + 9 * 2 + 12;
        break;
    case 1:
        PtrSize = (XMAX) * (YMAX)*2 + 2 + 9 * 2 + 13;
        break;
    case 2:
        PtrSize = (XMAX) * (YMAX)*24 + 2 + 9 * 2 + 17;
        break;
    case 3:
        PtrSize = (XMAX) * (YMAX)*25 + 2 + 9 * 2 + 17;
        break;
    }
    if ((PtrSize & 0xf) == 0) //Divisible by 32
    {
        PtrSize = (PtrSize >> 5);
    }
    else
    {
        PtrSize = (PtrSize >> 5) + 1;
    }
    *output = (uint32_t *)calloc(PtrSize, sizeof(uint32_t)); //Generate a continue memory

    //first 2 bits//
    *(*output) |= (MODE & 2) << 30; //color bit
    *(*output) |= (MODE & 1) << 30; //transparent bit
    Uint_ptr = 2;
    //
    //Width and Length//
    for (int i = 9 - 1; i >= 0; i--) //Width
    {
        uint32_t temp = Width & (1 << i);
        *(*output) |= temp << (32 - 2 - 9);

        Uint_ptr++;
    }
    for (int i = 9 - 1; i >= 0; i--) //Length
    {
        uint32_t temp = Length & (1 << i);
        *(*output) |= temp << (32 - 2 - 18);

        Uint_ptr++;
    }
    //
    switch (MODE)
    {
    case 0:
        //printf("%x\n", PtrSize);
        for (int i = 12 - 1; i >= 0; i--) //PtrSize
        {
            uint32_t temp = (PtrSize & (1 << i)) != 0;
            *(*output + ptr) |= temp << (31 - Uint_ptr);

            if (Uint_ptr != 31)

            {
                Uint_ptr++;
            }
            else
            {
                Uint_ptr = 0;
                ptr++;
            }
        }
        for (uint32_t y = 0; y < YMAX; y++)
        {
            for (uint32_t x = 0; x < XMAX; x++)
            {
                *(*output + ptr) |= data0[y][x] << (31 - Uint_ptr);
                //printf("ptr=%d,Uint_ptr=%d,*(*output + ptr)=%x\n", ptr, Uint_ptr, *(*output + ptr));
                if (Uint_ptr != 31)

                {
                    Uint_ptr++;
                }
                else
                {
                    Uint_ptr = 0;
                    ptr++;
                }
            }
        }
        break;
    case 1:
        //printf("%x\n", PtrSize);
        for (int i = 13 - 1; i >= 0; i--) //PtrSize
        {
            uint32_t temp = (PtrSize & (1 << i)) != 0;
            *(*output + ptr) |= temp << (31 - Uint_ptr);

            if (Uint_ptr != 31)

            {
                Uint_ptr++;
            }
            else
            {
                Uint_ptr = 0;
                ptr++;
            }
        }
        for (uint32_t y = 0; y < YMAX; y++)
        {
            for (uint32_t x = 0; x < XMAX; x++)
            {
                *(*output + ptr) |= tp[y][x] << (31 - Uint_ptr);
                //printf("ptr=%d,Uint_ptr=%d,*(*output + ptr)=%x\n", ptr, Uint_ptr, *(*output + ptr));
                if (Uint_ptr != 31)

                {
                    Uint_ptr++;
                }
                else
                {
                    Uint_ptr = 0;
                    ptr++;
                }
                *(*output + ptr) |= data0[y][x] << (31 - Uint_ptr);
                //printf("ptr=%d,Uint_ptr=%d,*(*output + ptr)=%x\n", ptr, Uint_ptr, *(*output + ptr));
                if (Uint_ptr != 31)

                {
                    Uint_ptr++;
                }
                else
                {
                    Uint_ptr = 0;
                    ptr++;
                }
            }
        }
        break;
    case 2:
        for (int i = 17 - 1; i >= 0; i--) //PtrSize
        {
            uint32_t temp = (PtrSize & (1 << i)) != 0;
            *(*output + ptr) |= temp << (31 - Uint_ptr);

            if (Uint_ptr != 31)

            {
                Uint_ptr++;
            }
            else
            {
                Uint_ptr = 0;
                ptr++;
            }
        }
        for (uint32_t y = 0; y < YMAX; y++)
        {
            for (uint32_t x = 0; x < XMAX; x++)
            {
                for (int i = 24 - 1; i >= 0; i--)
                {

                    *(*output + ptr) |= ((data1[y][x] >> i) & 1) << (31 - Uint_ptr);

                    if (Uint_ptr != 31)
                    {
                        Uint_ptr++;
                    }
                    else
                    {
                        Uint_ptr = 0;
                        ptr++;
                    }
                }
            }
        }
        break;
    case 3:
        printf("%x\n", PtrSize);
        printf("HERE %x, \n", *(*output));
        for (int i = 17 - 1; i >= 0; i--) //PtrSize
        {
            uint32_t temp = (PtrSize & (1 << i)) != 0;
            *(*output + ptr) |= temp << (31 - Uint_ptr);

            if (Uint_ptr != 31)

            {
                Uint_ptr++;
            }
            else
            {
                Uint_ptr = 0;
                ptr++;
            }
        }
        for (uint32_t y = 0; y < YMAX; y++)
        {
            for (uint32_t x = 0; x < XMAX; x++)
            {
                *(*output + ptr) |= tp[y][x] << (31 - Uint_ptr);
                //printf("ptr=%d,Uint_ptr=%d,*(*output + ptr)=%x\n", ptr, Uint_ptr, *(*output + ptr));
                if (Uint_ptr != 31)
                {
                    Uint_ptr++;
                }
                else
                {
                    Uint_ptr = 0;
                    ptr++;
                }
                for (int i = 24 - 1; i >= 0; i--)
                {

                    *(*output + ptr) |= ((data1[y][x] >> i) & 1) << (31 - Uint_ptr);

                    //printf("x=%d,y=%d,ptr=%d,Uint_ptr=%d,*(*output + ptr)=%x\n", x, y, ptr, Uint_ptr, *(*output + ptr));
                    if (Uint_ptr != 31)
                    {
                        Uint_ptr++;
                    }
                    else
                    {
                        Uint_ptr = 0;
                        ptr++;
                    }
                }
            }
        }
        break;
    }
}