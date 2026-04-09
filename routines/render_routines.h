#ifndef RENDER_ROUTINES_H
#define RENDER_ROUTINES_H

#include "screen_routines.h"
#include "../ghost/ghost.h"
#include "../player/pacman.h"

void backgroundUpdates(tBitMap *background, Position lastPos[][2], int bufferIdx, tBitMap *screenBuffer,
					   Ghost *orange, Ghost *blue, Ghost *red, Ghost *pink, Pacman *pac);

void bobUpdates(Ghost *blue, Ghost *red, Ghost *pink, Ghost *orange, Pacman *pac,
				Position lastPos[][2], int bufferIdx, tBitMap *tiles, tBitMap *screenBuffer,
				const UBYTE *tilesMask);

#endif // RENDER_ROUTINES_H
