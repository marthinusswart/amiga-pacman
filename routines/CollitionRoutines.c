#include "CollitionRoutines.h"

// Simple macro to get tile type at pixel X,Y
// Using bit-shifts (>> 4) instead of division ( / 16) for 68k speed!
#define GET_TILE(map, x, y) (map)[((y) >> 4) * 20 + ((x) >> 4)]

int canMove(UBYTE *map, short nextX, short nextY)
{
    UBYTE tl = GET_TILE(map, nextX, nextY);
    UBYTE tr = GET_TILE(map, nextX + 15, nextY);
    UBYTE bl = GET_TILE(map, nextX, nextY + 15);
    UBYTE br = GET_TILE(map, nextX + 15, nextY + 15);

    // KPrintF("Checking move to (%d, %d) - Tiles: TL:%d TR:%d BL:%d BR:%d\n", nextX, nextY, tl, tr, bl, br);

    // Check 4 corners of Pac-Man (assuming 16x16 sprite)
    if (tl == 1)
    {
        // KPrintF("Blocked by Top-Left wall!\n");
        return 0; // Top-Left
    }
    if (tr == 1)
    {
        // KPrintF("Blocked by Top-Right wall!\n");
        return 0; // Top-Right
    }
    if (bl == 1)
    {
        // KPrintF("Blocked by Bottom-Left wall!\n");
        return 0; // Bottom-Left
    }
    if (br == 1)
    {
        // KPrintF("Blocked by Bottom-Right wall!\n");
        return 0; // Bottom-Right
    }

    return 1; // Path is clear
}
