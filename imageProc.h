#include "GrayBMP.h"

#ifndef _IMAGEPROC_
#define _IMAGEPROC_

typedef enum searchDir {
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    Downright,
    NoneD
} searchDir;
void makeSmallDot(GrayBMP &bmp, int x, int y);
void makeBigDot(GrayBMP &bmp, int x, int y);
uint8_t SobelPixel(GrayBMP &original, GrayBMP &current, int x, int y);
//void EdgeDetect(GrayBMP &original, GrayBMP &current);
//void recursionSearch(GrayBMP &original, GrayBMP &current, searchDir state, searchDir direction, int CoorX, int CoorY);

#endif