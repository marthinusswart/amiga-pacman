#include "collision_routines.h"

// Simple macro to get tile type at pixel X,Y
// Using bit-shifts (>> 4) instead of division ( / 16) for 68k speed!
#define GET_TILE(map, x, y) (map)[((y) >> 4) * 20 + ((x) >> 4)]

int canMove(UBYTE *map, short *spriteX, short *spriteY, short nextX, short nextY)
{
    UBYTE tl = GET_TILE(map, nextX, nextY);
    UBYTE tr = GET_TILE(map, nextX + 15, nextY);
    UBYTE bl = GET_TILE(map, nextX, nextY + 15);
    UBYTE br = GET_TILE(map, nextX + 15, nextY + 15);

    // KPrintF("Checking move to (%d, %d) - Tiles: TL:%d TR:%d BL:%d BR:%d\n", nextX, nextY, tl, tr, bl, br);

    // Check 4 corners of Pac-Man (assuming 16x16 sprite)
    if (tl != 1 && tr != 1 && bl != 1 && br != 1)
    {
        if (spriteX)
            *spriteX = nextX;
        if (spriteY)
            *spriteY = nextY;
        return 1; // Path is clear
    }

    // Auto-alignment (corner cutting) logic if blocked
    if (spriteX != 0 && spriteY != 0)
    {
        short remX = nextX & 15;
        short remY = nextY & 15;
        short dx = nextX - *spriteX;
        short dy = nextY - *spriteY;

        if (dx > 0) // Moving RIGHT
        {
            if (tr == 1 && br != 1 && (16 - remY) < 5)
            {
                *spriteY += 1;
                *spriteX = nextX;
                return 1;
            }
            else if (tr != 1 && br == 1 && remY < 5)
            {
                *spriteY -= 1;
                *spriteX = nextX;
                return 1;
            }
        }
        else if (dx < 0) // Moving LEFT
        {
            if (tl == 1 && bl != 1 && (16 - remY) < 5)
            {
                *spriteY += 1;
                *spriteX = nextX;
                return 1;
            }
            else if (tl != 1 && bl == 1 && remY < 5)
            {
                *spriteY -= 1;
                *spriteX = nextX;
                return 1;
            }
        }
        else if (dy > 0) // Moving DOWN
        {
            if (bl == 1 && br != 1 && (16 - remX) < 5)
            {
                *spriteX += 1;
                *spriteY = nextY;
                return 1;
            }
            else if (bl != 1 && br == 1 && remX < 5)
            {
                *spriteX -= 1;
                *spriteY = nextY;
                return 1;
            }
        }
        else if (dy < 0) // Moving UP
        {
            if (tl == 1 && tr != 1 && (16 - remX) < 5)
            {
                *spriteX += 1;
                *spriteY = nextY;
                return 1;
            }
            else if (tl != 1 && tr == 1 && remX < 5)
            {
                *spriteX -= 1;
                *spriteY = nextY;
                return 1;
            }
        }
    }

    return 0; // Blocked completely
}

int isColliding(int spriteAX, int spriteAY, int spriteAWidth, int spriteAHeight, int spriteBX, int spriteBY, int spriteBWidth, int spriteBHeight)
{
    return (spriteAX < spriteBX + spriteBWidth && spriteAX + spriteAWidth > spriteBX && spriteAY < spriteBY + spriteBHeight && spriteAY + spriteAHeight > spriteBY);
}
