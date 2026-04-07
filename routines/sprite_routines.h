#ifndef SPRITE_ROUTINES_H
#define SPRITE_ROUTINES_H

#include "player/pacman.h"
#include "sprite/sprite_ext.h"
#include "ghost/ghost.h"
#include "routines/screen_routines.h"
#include "map/stages.h"
#include "support/gcc8_c_support.h"

short setupPacman(Pacman **pacman);
short setupBlueGhost(Ghost **blueGhost);
short setupRedGhost(Ghost **redGhost);
short setupPinkGhost(Ghost **pinkGhost);
short setupOrangeGhost(Ghost **orangeGhost);
short setupStartText(Sprite **startText);
short setupGameOverText(Sprite **gameOverText);

#endif // SPRITE_ROUTINES_H