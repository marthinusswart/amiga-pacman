#include "pathfinding_routines.h"

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

// Helper function to evaluate the 4 surrounding tiles and pick the shortest path to a target
static void calculateNextDirection(Ghost *g, int targetX, int targetY)
{
    int best_dist = 0x7FFFFFFF; // Max int
    Direction nextDirection = g->direction;
    Direction oppositeDirection = getOppositeDirection(g->direction);

    // Evaluate all 4 possible directions (LEFT, UP, RIGHT, DOWN)
    for (int i = 0; i < 4; i++)
    {
        Direction checkDirection = (Direction)i;

        // 1. Is this direction the opposite of the current one? (Disallowed)
        if (checkDirection == oppositeDirection)
            continue;

        int nextX = g->x;
        int nextY = g->y;

        switch (checkDirection)
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
        if (g->currentMap && !canMove(g->currentMap, 0, 0, nextX, nextY))
            continue;

        // 3. Calculate distance squared
        int dx = nextX - targetX;
        int dy = nextY - targetY;
        int dist_sq = (dx * dx) + (dy * dy); // No expensive sqrt needed

        if (dist_sq < best_dist)
        {
            best_dist = dist_sq;
            nextDirection = checkDirection;
        }
    }

    // If no valid direction was found, the ghost is in a dead-end.
    // In this case, we allow it to break the "no-reverse" rule to get out.
    if (best_dist == 0x7FFFFFFF)
    {
        nextDirection = oppositeDirection;
    }

    g->direction = nextDirection;
}

void updateChaseGhostDirection(Ghost *g, Pacman *p)
{
    // Only calculate a new path when perfectly aligned to a 16x16 tile
    if ((g->x & 15) == 0 && (g->y & 15) == 0)
    {
#ifdef DEBUG_PATHFINDING
        ULONG startTime = timerGetPrec();
#endif

        calculateNextDirection(g, p->x, p->y);

#ifdef DEBUG_PATHFINDING
        ULONG endTime = timerGetPrec();
        ULONG delta = timerGetDelta(startTime, endTime);
        char timeStr[15];
        timerFormatPrec(timeStr, delta);
        KPrintF("updateChaseGhostDirection duration: %s\n", timeStr);
#endif
    }
}

void updateUnpredictableGhostDirection(Ghost *g, Pacman *p, Ghost *blinky)
{
    // Only calculate a new path when perfectly aligned to a 16x16 tile
    if ((g->x & 15) == 0 && (g->y & 15) == 0)
    {
#ifdef DEBUG_PATHFINDING
        ULONG startTime = timerGetPrec();
#endif

        // 1. Calculate pivot point (2 tiles ahead of Pac-Man)
        int pivotX = p->x;
        int pivotY = p->y;
        switch (p->direction)
        {
        case LEFT:
            pivotX -= 32;
            break;
        case UP:
            pivotY -= 32;
            break;
        case RIGHT:
            pivotX += 32;
            break;
        case DOWN:
            pivotY += 32;
            break;
        }

        // 2. Vector from Blinky to Pivot
        int vecX = pivotX - blinky->x;
        int vecY = pivotY - blinky->y;

        // 3. Target is Pivot + Vector (which is mathematically 2 * Pivot - Blinky)
        calculateNextDirection(g, pivotX + vecX, pivotY + vecY);

#ifdef DEBUG_PATHFINDING
        ULONG endTime = timerGetPrec();
        ULONG delta = timerGetDelta(startTime, endTime);
        char timeStr[15];
        timerFormatPrec(timeStr, delta);
        KPrintF("updateUnpredictableGhostDirection duration: %s\n", timeStr);
#endif
    }
}

void updateAmbushGhostDirection(Ghost *g, Pacman *p)
{
    // Only calculate a new path when perfectly aligned to a 16x16 tile
    if ((g->x & 15) == 0 && (g->y & 15) == 0)
    {
#ifdef DEBUG_PATHFINDING
        ULONG startTime = timerGetPrec();
#endif

        // Target is 4 tiles (64 pixels) ahead of Pac-Man
        int targetX = p->x;
        int targetY = p->y;

        switch (p->direction)
        {
        case LEFT:
            targetX -= 64;
            break;
        case UP:
            targetY -= 64;
            // targetX -= 64; // Arcade accuracy note: Pinky also had the UP overflow bug! Uncomment to emulate it.
            break;
        case RIGHT:
            targetX += 64;
            break;
        case DOWN:
            targetY += 64;
            break;
        }

        calculateNextDirection(g, targetX, targetY);

#ifdef DEBUG_PATHFINDING
        ULONG endTime = timerGetPrec();
        ULONG delta = timerGetDelta(startTime, endTime);
        char timeStr[15];
        timerFormatPrec(timeStr, delta);
        KPrintF("updateAmbushGhostDirection duration: %s\n", timeStr);
#endif
    }
}

void updateCowardGhostDirection(Ghost *g, Pacman *p)
{
    // Only calculate a new path when perfectly aligned to a 16x16 tile
    if ((g->x & 15) == 0 && (g->y & 15) == 0)
    {
#ifdef DEBUG_PATHFINDING
        ULONG startTime = timerGetPrec();
#endif

        int dx = g->x - p->x;
        int dy = g->y - p->y;
        int dist_sq = (dx * dx) + (dy * dy);

        int targetX, targetY;

        // Check if distance is greater than 8 tiles
        // 8 tiles * 16 pixels = 128 pixels. 128^2 = 16384.
        if (dist_sq > 16384)
        {
            // Far away: target Pac-Man directly
            targetX = p->x;
            targetY = p->y;
        }
        else
        {
            // Close by (panicking): retreat to his home corner (Bottom-Left)
            targetX = 64;
            targetY = 192;
        }

        calculateNextDirection(g, targetX, targetY);

#ifdef DEBUG_PATHFINDING
        ULONG endTime = timerGetPrec();
        ULONG delta = timerGetDelta(startTime, endTime);
        char timeStr[15];
        timerFormatPrec(timeStr, delta);
        KPrintF("updateCowardGhostDirection duration: %s\n", timeStr);
#endif
    }
}