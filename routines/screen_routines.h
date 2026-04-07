#ifndef SCREEN_ROUTINES_H
#define SCREEN_ROUTINES_H

#include <exec/types.h>

APTR GetVBR(void);
void SetInterruptHandler(APTR interrupt);
APTR GetInterruptHandler(void);

void WaitVbl(void);
void WaitLine(USHORT line);
void WaitBlt(void);

short screenScanDefault(USHORT **copListEnd_out);
// Calculates the X and Y pixel coordinates of a sprite within a tileset
void calculateSpriteLocation(int row, int col, int spriteWidth, int spriteHeight, int tilesetWidth, int tilesetHeight, int *spriteX, int *spriteY);
BOOL isValidSpriteLocation(int x, int y, int spriteWidth, int spriteHeight, int screenWidth, int screenHeight);

#endif // SCREEN_ROUTINES_H