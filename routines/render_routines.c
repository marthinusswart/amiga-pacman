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

void bobUpdates(Ghost *blue, Ghost *red, Ghost *pink, Ghost *orange, Pacman *pac,
				Position lastPos[][2], int bufferIdx, tBitMap *tiles, tBitMap *screenBuffer,
				const UBYTE *tilesMask)
{
	Sprite *blueSprite = NULL;
	blue->getSprite(blue, blue->direction, &blueSprite);
	Sprite *redSprite = NULL;
	red->getSprite(red, red->direction, &redSprite);
	Sprite *pinkSprite = NULL;
	pink->getSprite(pink, pink->direction, &pinkSprite);
	Sprite *orangeSprite = NULL;
	orange->getSprite(orange, orange->direction, &orangeSprite);
	Sprite *pacSprite = NULL;
	pac->getSprite(pac, pac->direction, &pacSprite);

	// Save the locations we are about to draw to, so we can erase them next time this buffer is active
	lastPos[BLUE][bufferIdx].x = blue->x;
	lastPos[BLUE][bufferIdx].y = blue->y;

	lastPos[RED][bufferIdx].x = red->x;
	lastPos[RED][bufferIdx].y = red->y;

	lastPos[PINK][bufferIdx].x = pink->x;
	lastPos[PINK][bufferIdx].y = pink->y;

	lastPos[ORANGE][bufferIdx].x = orange->x;
	lastPos[ORANGE][bufferIdx].y = orange->y;

	lastPos[PACMAN][bufferIdx].x = pac->x;
	lastPos[PACMAN][bufferIdx].y = pac->y;

	if (orangeSprite)
		blitCopyMask(
			tiles, orangeSprite->x, orangeSprite->y,
			screenBuffer, orange->x, orange->y,
			orange->width, orange->height,
			tilesMask);

	if (blueSprite)
		blitCopyMask(
			tiles, blueSprite->x, blueSprite->y,
			screenBuffer, blue->x, blue->y,
			blue->width, blue->height,
			tilesMask);

	if (redSprite)
		blitCopyMask(
			tiles, redSprite->x, redSprite->y,
			screenBuffer, red->x, red->y,
			red->width, red->height,
			tilesMask);

	if (pinkSprite)
		blitCopyMask(
			tiles, pinkSprite->x, pinkSprite->y,
			screenBuffer, pink->x, pink->y,
			pink->width, pink->height,
			tilesMask);

	if (pacSprite)
		blitCopyMask(
			tiles, pacSprite->x, pacSprite->y,
			screenBuffer, pac->x, pac->y,
			pac->width, pac->height,
			tilesMask);
}
