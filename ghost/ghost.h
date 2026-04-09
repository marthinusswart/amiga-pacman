#ifndef GHOST_H
#define GHOST_H

#include <exec/types.h>
#include <hardware/custom.h>
#include <proto/exec.h>
#include "support/gcc8_c_support.h"
#include "routines/screen_routines.h"
#include "routines/collision_routines.h"
#include "routines/memory_routines.h"
#include "sprite/sprite_ext.h"

typedef struct Ghost
{
    int x;
    int y;
    int prevX;
    int prevY;
    int width;
    int height;
    int speed;
    BOOL movedPreviousFrame;
    Direction direction;
    UBYTE *currentMap;
    Sprite downSprite;
    Sprite upSprite;
    Sprite leftSprite;
    Sprite rightSprite;

    // Function pointer to emulate a method call
    void (*moveGhost)(struct Ghost *g, Direction dir);
    void (*addSprite)(struct Ghost *g, Direction dir, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData);
    short (*getSprite)(struct Ghost *g, Direction dir, Sprite **sprite_out);
    void (*setMap)(struct Ghost *g, UBYTE *map);
} Ghost;

short createGhost(Ghost **g_out, int x, int y, int width, int height);

#endif // GHOST_H
