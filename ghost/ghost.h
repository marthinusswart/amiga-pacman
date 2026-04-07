#pragma once

#ifndef GHOST_H
#define GHOST_H

#include <exec/types.h>
#include <hardware/custom.h>
#include <proto/exec.h>
#include "support/gcc8_c_support.h"
#include "routines/ScreenRoutines.h"
#include "routines/CollitionRoutines.h"
#include "routines/MemoryRoutines.h"
#include "sprite/sprite.h"

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
    void (*addSprite)(struct Ghost *g, Direction dir, int spriteX, int spriteY, int width, int height);
    Sprite *(*getSprite)(struct Ghost *g, Direction dir);
    void (*setMap)(struct Ghost *g, UBYTE *map);
} Ghost;

short createGhost(Ghost **g_out, int x, int y, int width, int height);

#endif // GHOST_H
