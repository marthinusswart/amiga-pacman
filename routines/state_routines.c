#include "state_routines.h"

UBYTE gameState[256];

short initializeGameState()
{
    for (int i = 0; i < 256; i++)
    {
        gameState[i] = OFF; // Default state for all tiles
    }
    return 0; // Success
}

short updateGameState(int state, int value)
{
    gameState[state] = value;
    return 0; // Success
}

UBYTE getGameState(int state)
{
    return gameState[state];
}