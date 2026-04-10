#pragma once

#ifndef PACMAN_H
#define PACMAN_H

#include <exec/types.h>
#include <hardware/custom.h>
#include <proto/exec.h>
#include "support/gcc8_c_support.h"
#include "routines/screen_routines.h"
#include "routines/collision_routines.h"
#include "routines/memory_routines.h"
#include "sprite/sprite_ext.h"

// Forward declaration to avoid circular dependency with ghost.h
typedef struct Ghost Ghost;

typedef struct Pacman
{
    int x;
    int y;
    int prevX;
    int prevY;
    int width;
    int height;
    Direction direction;
    const UBYTE *currentMap;
    Sprite downSprite;
    Sprite upSprite;
    Sprite leftSprite;
    Sprite rightSprite;

    // Function pointer to emulate a method call
    void (*movePacman)(struct Pacman *p, Direction dir);
    void (*addSprite)(struct Pacman *p, Direction dir, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData);
    short (*getSprite)(struct Pacman *p, Direction dir, Sprite **sprite_out);
    void (*setMap)(struct Pacman *p, const UBYTE *map);
    int (*isPacmanColliding)(struct Pacman *p, struct Ghost *redGhost, struct Ghost *blueGhost, struct Ghost *pinkGhost, struct Ghost *orangeGhost);
} Pacman;

short createPacman(Pacman **p_out, int x, int y, int width, int height);

#endif // PACMAN_H