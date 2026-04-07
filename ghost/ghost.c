#include "ghost.h"

// Private forward declaration
static void moveGhost(Ghost *g, Direction direction);
static void addSprite(Ghost *g, Direction direction, int spriteX, int spriteY, int width, int height);
static Sprite *getSprite(Ghost *g, Direction direction);
static void setMap(Ghost *g, UBYTE *map);

short createGhost(Ghost **g_out, int x, int y, int width, int height)
{
    if (!g_out)
        return -1;

    Ghost *g = (Ghost *)AllocMem(sizeof(Ghost), MEMF_CHIP | MEMF_CLEAR);
    if (!g)
        return -1; // Memory allocation failed

    g->x = x;
    g->y = y;
    g->width = width;
    g->height = height;
    g->prevX = x;
    g->prevY = y;
    g->speed = 1; // Default speed, can be modified later
    g->direction = RIGHT;
    g->moveGhost = moveGhost;      // Assign the function pointer
    g->addSprite = addSprite;      // Assign the function pointer
    g->getSprite = getSprite;      // Assign the function pointer
    g->setMap = setMap;            // Assign the function pointer
    g->currentMap = NULL;          // Initialize safely
    g->movedPreviousFrame = FALSE; // Initialize movement flag

    *g_out = g; // Assign to the caller's pointer
    return 0;   // Success
}

static void moveGhost(Ghost *g, Direction direction)
{
    if (g->speed == 1 && g->movedPreviousFrame)
    {
        // Skip this move to create a slower movement effect
        g->movedPreviousFrame = FALSE; // Reset for the next frame
        return;
    }

    g->prevX = g->x;
    g->prevY = g->y;
    g->direction = direction;
    g->movedPreviousFrame = TRUE; // Set the flag for the next frame

    short nextX = g->x;
    short nextY = g->y;

    switch (direction)
    {
    case LEFT:
        nextX -= 1;
        break;
    case UP:
        nextY -= 1;
        break;
    case RIGHT:
        nextX += 1;
        break;
    case DOWN:
        nextY += 1;
        break;
    default:
        break;
    }

    if (!isValidSpriteLocation(nextX, nextY, g->width, g->height, 320, 256))
    {
        // Invalid location, do nothing
    }
    else if (g->currentMap)
    {
        short sx = g->x;
        short sy = g->y;
        if (canMove(g->currentMap, &sx, &sy, nextX, nextY))
        {
            g->x = sx;
            g->y = sy;
        }
    }
    else
    {
        g->x = nextX;
        g->y = nextY;
    }
}

static void addSprite(Ghost *g, Direction direction, int spriteX, int spriteY, int width, int height)
{
    Sprite *sprite = NULL;
    switch (direction)
    {
    case LEFT:
        sprite = &g->leftSprite;
        break;
    case UP:
        sprite = &g->upSprite;
        break;
    case RIGHT:
        sprite = &g->rightSprite;
        break;
    case DOWN:
        sprite = &g->downSprite;
        break;
    default:
        return; // Invalid direction
    }
    sprite->x = spriteX;
    sprite->y = spriteY;
    sprite->width = width;
    sprite->height = height;
    sprite->spriteData = (const UBYTE *)pacman_tiles2; // Example: all sprites use the same data for now
}

static Sprite *getSprite(Ghost *g, Direction direction)
{
    switch (direction)
    {
    case LEFT:
        return &g->leftSprite;
    case UP:
        return &g->upSprite;
    case RIGHT:
        return &g->rightSprite;
    case DOWN:
        return &g->downSprite;
    default:
        return NULL; // Invalid direction
    }
}

static void setMap(Ghost *g, UBYTE *map)
{
    g->currentMap = map;
}
