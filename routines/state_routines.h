#ifndef STATE_ROUTINES_H
#define STATE_ROUTINES_H

#include "state/state_ext.h"

// clang-format off
#define START_GAME_TEXT         0
#define PLAYING_STATE           1
#define CLEARED_START_TEXT      2
#define GAME_OVER_TEXT          3
#define CLEAR_GAME_OVER_TEXT    4
#define PACMAN_DEBUG_MODE       5
#define FREEZE_GHOSTS           6
// clang-format on

short initializeGameState();
short updateGameState(int state, int value);
UBYTE getGameState(int state);

#endif // STATE_ROUTINES_H