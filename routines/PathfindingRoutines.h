#ifndef PATHFINDINGROUTINES_H
#define PATHFINDINGROUTINES_H

#include "ghost/ghost.h"
#include "player/pacman.h"

// Calculates the next best direction for the ghost based on Pac-Man's location
void updateGhostDirection(Ghost *g, Pacman *p);

#endif // PATHFINDINGROUTINES_H