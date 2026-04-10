#ifndef ALPHANUMERIC_ROUTINES_H
#define ALPHANUMERIC_ROUTINES_H

#include "sprite_routines.h"

short setupNumbers(Sprite *numericSprites[10], const UBYTE *spriteTileData);

void displayNumbers(int score, int startX, int startY, tBitMap *tiles, tBitMap *screenBuffer,
                    const UBYTE *tilesMask, Sprite *numericSprites[10], int leftPadding);

#endif // ALPHANUMERIC_ROUTINES_H