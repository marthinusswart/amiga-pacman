#include "ghost.h"

// Private forward declaration
static void moveGhost(Ghost *g, Direction direction);
static void addSprite(Ghost *g, Direction direction, int spriteX, int spriteY, int width, int height);
static Sprite *getSprite(Ghost *g, Direction direction);
static void setMap(Ghost *g, UBYTE *map);

Ghost *createGhost(int x, int y, int width, int height)
{
    Ghost *g = (Ghost *)AllocMem(sizeof(Ghost), MEMF_CHIP | MEMF_CLEAR);
    g->x = x;
    g->y = y;
    g->width = width;
    g->height = height;
    g->prevX = x;
    g->prevY = y;
    g->direction = RIGHT;
    g->moveGhost = moveGhost; // Assign the function pointer
    g->addSprite = addSprite; // Assign the function pointer
    g->getSprite = getSprite; // Assign the function pointer
    g->setMap = setMap;       // Assign the function pointer
    g->currentMap = NULL;     // Initialize safely
    return g;
}

static void moveGhost(Ghost *g, Direction direction)
{
    g->prevX = g->x;
    g->prevY = g->y;
    g->direction = direction;
    switch (direction)
    {
    case LEFT:
        g->x -= 1;
        break;
    case UP:
        g->y -= 1;
        break;
    case RIGHT:
        g->x += 1;
        break;
    case DOWN:
        g->y += 1;
        break;
    default:
        break;
    }

    if (!isValidSpriteLocation(g->x, g->y, g->width, g->height, 320, 256))
    {
        // Invalid location, revert to previous position
        g->x = g->prevX;
        g->y = g->prevY;
    }
    else if (g->currentMap && !canMove(g->currentMap, g->x, g->y))
    {
        // Collision detected, revert to previous position
        g->x = g->prevX;
        g->y = g->prevY;
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
