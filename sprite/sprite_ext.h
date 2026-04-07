#ifndef SPRITE_EXT_H
#define SPRITE_EXT_H

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

short createSprite(Sprite **sprite_out);

#endif // SPRITE_EXT_H