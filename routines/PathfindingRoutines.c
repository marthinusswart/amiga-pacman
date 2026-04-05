#include "PathfindingRoutines.h"

#define DEBUG_PATHFINDING_OFF

#ifdef DEBUG_PATHFINDING
#include <ace/managers/timer.h>
#include "support/gcc8_c_support.h"
#endif

static Direction getOppositeDirection(Direction dir)
{
    switch (dir)
    {
    case LEFT:
        return RIGHT;
    case UP:
        return DOWN;
    case RIGHT:
        return LEFT;
    case DOWN:
        return UP;
    default:
        return RIGHT;
    }
}

void updateGhostDirection(Ghost *g, Pacman *p)
{
    // Only calculate a new path when perfectly aligned to a 16x16 tile
    // Using bitwise AND (& 15) instead of modulo (% 16) for 68k speed!
    if ((g->x & 15) == 0 && (g->y & 15) == 0)
    {
#ifdef DEBUG_PATHFINDING
        ULONG startTime = timerGetPrec();
#endif

        int best_dist = 0x7FFFFFFF; // Max int
        Direction next_dir = g->direction;
        Direction opposite_dir = getOppositeDirection(g->direction);

        // Evaluate all 4 possible directions (LEFT, UP, RIGHT, DOWN)
        for (int i = 0; i < 4; i++)
        {
            Direction check_dir = (Direction)i;

            // 1. Is this direction the opposite of the current one? (Disallowed)
            if (check_dir == opposite_dir)
                continue;

            int nextX = g->x;
            int nextY = g->y;

            switch (check_dir)
            {
            case LEFT:
                nextX -= 16;
                break;
            case UP:
                nextY -= 16;
                break;
            case RIGHT:
                nextX += 16;
                break;
            case DOWN:
                nextY += 16;
                break;
            }

            // 2. Check for collisions using the map
            if (g->currentMap && !canMove(g->currentMap, nextX, nextY))
                continue;

            // 3. Calculate distance squared
            int dx = nextX - p->x;
            int dy = nextY - p->y;
            int dist_sq = (dx * dx) + (dy * dy); // No expensive sqrt needed

            if (dist_sq < best_dist)
            {
                best_dist = dist_sq;
                next_dir = check_dir;
            }
        }

        // If no valid direction was found, the ghost is in a dead-end.
        // In this case, we allow it to break the "no-reverse" rule to get out.
        if (best_dist == 0x7FFFFFFF)
        {
            next_dir = opposite_dir;
        }

        g->direction = next_dir;

#ifdef DEBUG_PATHFINDING
        ULONG endTime = timerGetPrec();
        ULONG delta = timerGetDelta(startTime, endTime);
        char timeStr[15];
        timerFormatPrec(timeStr, delta);
        KPrintF("updateGhostDirection duration: %s\n", timeStr);
#endif
    }
}