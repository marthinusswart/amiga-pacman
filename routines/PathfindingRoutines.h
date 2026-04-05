#ifndef PATHFINDINGROUTINES_H
#define PATHFINDINGROUTINES_H

#include "ghost/ghost.h"
#include "player/pacman.h"

// Calculates the next best direction for the ghost based on Pac-Man's location
void updateChaseGhostDirection(Ghost *g, Pacman *p);

// Calculates Inky's unpredictable direction using a vector from Blinky through Pac-Man
void updateUnpredictableGhostDirection(Ghost *g, Pacman *p, Ghost *blinky);

// Calculates Pinky's ambush direction by targeting 4 tiles ahead of Pac-Man
void updateAmbushGhostDirection(Ghost *g, Pacman *p);

// Calculates Clyde's coward direction by targeting Pac-Man unless too close, then retreating to his corner
void updateCowardGhostDirection(Ghost *g, Pacman *p);

#endif // PATHFINDINGROUTINES_H