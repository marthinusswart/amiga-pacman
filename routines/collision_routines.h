#ifndef COLLISION_ROUTINES_H
#define COLLISION_ROUTINES_H

#include <exec/types.h>
#include "support/gcc8_c_support.h"
#include "map/stages.h"
#include "player/pacman.h"
#include "ghost/ghost.h"

int canMove(const UBYTE *map, short *spriteX, short *spriteY, short nextX, short nextY);
int isColliding(int spriteAX, int spriteAY, int spriteAWidth, int spriteAHeight, int spriteBX, int spriteBY, int spriteBWidth, int spriteBHeight, int overlap);
int tileHasPellet(const UBYTE *map, int x, int y);

#endif // COLLISION_ROUTINES_H