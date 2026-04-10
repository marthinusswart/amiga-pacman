#include "alphanumeric_routines.h"
#include <ace/managers/blit.h> // For blitCopyMask
#include <mini_std/printf.h>   // For sprintf

short setupNumbers(Sprite *numericSprites[10], const UBYTE *spriteTileData)
{
    for (int i = 0; i < 10; ++i)
    {
        setupSprite(&numericSprites[i], spriteTileData, 0, i, 8, 8, 160, 160);
    }

    return 0;
}

void displayNumbers(int score, int startX, int startY, tBitMap *tiles, tBitMap *screenBuffer,
                    const UBYTE *tilesMask, Sprite *numericSprites[10], int leftPadding)
{
    char scoreStr[5]; // Max 4 digits + null terminator
    // Format the score as a 4-digit string, left-padded with zeros.
    // For example, if score is 25, scoreStr will be "0025".
    sprintf(scoreStr, "%04d", score);

    int currentX = startX;
    for (int i = 0; i < leftPadding; ++i)
    {
        int digit = scoreStr[i] - '0'; // Convert char digit ('0'-'9') to int (0-9)
        if (digit >= 0 && digit <= 9)
        {
            const Sprite *digitSprite = numericSprites[digit];
            if (digitSprite)
            {
                blitCopyMask(
                    tiles, digitSprite->x, digitSprite->y,
                    screenBuffer, currentX, startY,
                    digitSprite->width, digitSprite->height,
                    tilesMask);
            }
        }
        currentX += 8; // Move to the right for the next digit (assuming 8px wide digits)
    }
}
