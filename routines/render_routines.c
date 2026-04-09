#include "render_routines.h"
#include <ace/managers/blit.h>

void backgroundUpdates(tBitMap *background, Position lastPos[][2], int bufferIdx, tBitMap *screenBuffer,
					   Ghost *orange, Ghost *blue, Ghost *red, Ghost *pink, Pacman *pac)
{
	blitCopy(background, lastPos[ORANGE][bufferIdx].x, lastPos[ORANGE][bufferIdx].y,
			 screenBuffer, lastPos[ORANGE][bufferIdx].x, lastPos[ORANGE][bufferIdx].y,
			 orange->width, orange->height, MINTERM_COOKIE);

	blitCopy(background, lastPos[BLUE][bufferIdx].x, lastPos[BLUE][bufferIdx].y,
			 screenBuffer, lastPos[BLUE][bufferIdx].x, lastPos[BLUE][bufferIdx].y,
			 blue->width, blue->height, MINTERM_COOKIE);

	blitCopy(background, lastPos[RED][bufferIdx].x, lastPos[RED][bufferIdx].y,
			 screenBuffer, lastPos[RED][bufferIdx].x, lastPos[RED][bufferIdx].y,
			 red->width, red->height, MINTERM_COOKIE);

	blitCopy(background, lastPos[PINK][bufferIdx].x, lastPos[PINK][bufferIdx].y,
			 screenBuffer, lastPos[PINK][bufferIdx].x, lastPos[PINK][bufferIdx].y,
			 pink->width, pink->height, MINTERM_COOKIE);

	blitCopy(background, lastPos[PACMAN][bufferIdx].x, lastPos[PACMAN][bufferIdx].y,
			 screenBuffer, lastPos[PACMAN][bufferIdx].x, lastPos[PACMAN][bufferIdx].y,
			 pac->width, pac->height, MINTERM_COOKIE);
}
