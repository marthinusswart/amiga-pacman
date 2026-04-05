#ifndef SPRITE_H
#define SPRITE_H

#include <exec/types.h>

typedef enum
{
    LEFT,
    UP,
    RIGHT,
    DOWN
} Direction;

typedef struct Sprite
{
    int x;
    int y;
    int width;
    int height;
    const UBYTE *spriteData;
} Sprite;

#endif // SPRITE_H