#pragma once

#ifndef PACMAN_H
#define PACMAN_H

#include <exec/types.h>
#include <hardware/custom.h>
#include <proto/exec.h>
#include "support/gcc8_c_support.h"
#include "routines/ScreenRoutines.h"
#include "routines/CollitionRoutines.h"
#include "routines/MemoryRoutines.h"
#include "sprite/sprite.h"

typedef struct Pacman
{
    int x;
    int y;
    int prevX;
    int prevY;
    int width;
    int height;
    Direction direction;
    UBYTE *currentMap;
    Sprite downSprite;
    Sprite upSprite;
    Sprite leftSprite;
    Sprite rightSprite;

    // Function pointer to emulate a method call
    void (*movePacman)(struct Pacman *p, Direction dir);
    void (*addSprite)(struct Pacman *p, Direction dir, int spriteX, int spriteY, int width, int height);
    short (*getSprite)(struct Pacman *p, Direction dir, Sprite **sprite_out);
    void (*setMap)(struct Pacman *p, UBYTE *map);
} Pacman;

short createPacman(Pacman **p_out, int x, int y, int width, int height);

#endif // PACMAN_H