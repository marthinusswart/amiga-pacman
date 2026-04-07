#ifndef COLLISION_ROUTINES_H
#define COLLISION_ROUTINES_H

#include <exec/types.h>
#include "support/gcc8_c_support.h"
#include "map/Stages.h"

int canMove(UBYTE *map, short *spriteX, short *spriteY, short nextX, short nextY);

#endif // COLLISION_ROUTINES_H