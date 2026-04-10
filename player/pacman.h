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
#include "routines/log_routines.h"
#include "sprite/sprite_ext.h"

#define PACMAN_ANIMATION_FRAMES 4
#define MAX_PACMAN_ANIMATION_SPEED 10

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
    int speed;
    int currentAnimationIndex;
    int animationCounter; // Counter to control animation speed
    BOOL wasMoving;       // Flag to track if Pacman was moving in the last update
    BOOL isAnimated;      // Flag to control whether Pacman should animate or stay static

    Direction direction;
    Direction lastDirection;
    const UBYTE *currentMap;
    Sprite downSprites[PACMAN_ANIMATION_FRAMES];
    Sprite upSprites[PACMAN_ANIMATION_FRAMES];
    Sprite leftSprites[PACMAN_ANIMATION_FRAMES];
    Sprite rightSprites[PACMAN_ANIMATION_FRAMES];

    // Function pointer to emulate a method call
    void (*movePacman)(struct Pacman *p, Direction dir);
    void (*addSprite)(struct Pacman *p, Direction dir, int index, int spriteX, int spriteY,
                      int width, int height, const UBYTE *spriteTileData);
    short (*getSprite)(struct Pacman *p, Direction dir, Sprite **sprite_out);
    short (*getSpriteIdx)(struct Pacman *p, Direction dir, int index, Sprite **sprite_out);
    void (*setMap)(struct Pacman *p, const UBYTE *map);
    int (*isPacmanColliding)(struct Pacman *p, struct Ghost *redGhost,
                             struct Ghost *blueGhost, struct Ghost *pinkGhost, struct Ghost *orangeGhost);
    void (*pulseCheck)(struct Pacman *p);
} Pacman;

short createPacman(Pacman **p_out, int x, int y, int width, int height);

#endif // PACMAN_H