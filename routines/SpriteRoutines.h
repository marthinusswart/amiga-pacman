#ifndef SPRITEROUTINES_H
#define SPRITEROUTINES_H

#include "player/pacman.h"
#include "sprite/sprite_ext.h"
#include "ghost/ghost.h"
#include "routines/ScreenRoutines.h"
#include "map/Stages.h"
#include "support/gcc8_c_support.h"

short setupPacman(Pacman **pacman);
short setupBlueGhost(Ghost **blueGhost);
short setupRedGhost(Ghost **redGhost);
short setupPinkGhost(Ghost **pinkGhost);
short setupOrangeGhost(Ghost **orangeGhost);
short setupStartText(Sprite **startText);

#endif // SPRITEROUTINES_H