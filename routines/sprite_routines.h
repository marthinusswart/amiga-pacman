#ifndef SPRITE_ROUTINES_H
#define SPRITE_ROUTINES_H

#include "player/pacman.h"
#include "sprite/sprite_ext.h"
#include "ghost/ghost.h"
#include "routines/screen_routines.h"
#include "map/stages.h"
#include "support/gcc8_c_support.h"

short setupPacman(Pacman **pacman, const UBYTE *spriteTileData);
short setupBlueGhost(Ghost **blueGhost, const UBYTE *spriteTileData);
short setupRedGhost(Ghost **redGhost, const UBYTE *spriteTileData);
short setupPinkGhost(Ghost **pinkGhost, const UBYTE *spriteTileData);
short setupOrangeGhost(Ghost **orangeGhost, const UBYTE *spriteTileData);
short setupStartText(Sprite **startText, const UBYTE *spriteTileData);
short setupGameOverText(Sprite **gameOverText, const UBYTE *spriteTileData);
short setupHighScoreText(Sprite **highScoreText, const UBYTE *spriteTileData);
short setupPowerPill(Sprite **powerPill, const UBYTE *spriteTileData);
short setupPellets(Sprite **pellet, const UBYTE *spriteTileData);
short setupSprite(Sprite **sprite, const UBYTE *spriteTileData, int row, int col,
				  int width, int height, int tilesetWidth, int tilesetHeight);

int addPowerPillsToMap(Sprite *pill, tBitMap *background, tBitMap *pacmanTiles,
					   tBitMap **screenBuffers, const UBYTE *tilesMask, const UBYTE *mapData);
int addPelletsToMap(Sprite *pellet, UBYTE *pelletsOnMap, tBitMap *background, tBitMap *pacmanTiles,
					tBitMap **screenBuffers, const UBYTE *tilesMask, const UBYTE *mapData);

void ghostUpdates(Pacman *pacman, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost);

void updateSpriteMaps(Pacman *pacman, Ghost *blueGhost, Ghost *redGhost, Ghost *pinkGhost,
					  Ghost *orangeGhost, const UBYTE *mapData);

#endif // SPRITE_ROUTINES_H