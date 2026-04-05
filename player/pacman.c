#include "pacman.h"

// Private forward declaration
static void movePacman(Pacman *p, Direction direction);
static void addSprite(Pacman *p, Direction direction, int spriteX, int spriteY, int width, int height);
static Sprite *getSprite(Pacman *p, Direction direction);
static void setMap(Pacman *p, UBYTE *map);

Pacman *createPacman(int x, int y, int width, int height)
{
    Pacman *p = (Pacman *)AllocMem(sizeof(Pacman), MEMF_CHIP | MEMF_CLEAR);
    p->x = x;
    p->y = y;
    p->width = width;
    p->height = height;
    p->prevX = x;
    p->prevY = y;
    p->direction = RIGHT;
    p->movePacman = movePacman; // Assign the function pointer
    p->addSprite = addSprite;   // Assign the function pointer
    p->getSprite = getSprite;   // Assign the function pointer
    p->setMap = setMap;         // Assign the function pointer
    p->currentMap = NULL;       // Initialize safely
    return p;
}

static void movePacman(Pacman *p, Direction direction)
{
    p->prevX = p->x;
    p->prevY = p->y;
    p->direction = direction;
    switch (direction)
    {
    case LEFT:
        p->x -= 1;
        break;
    case UP:
        p->y -= 1;
        break;
    case RIGHT:
        p->x += 1;
        break;
    case DOWN:
        p->y += 1;
        break;
    default:
        break;
    }

    if (!isValidSpriteLocation(p->x, p->y, p->width, p->height, 320, 256))
    {
        // Invalid location, revert to previous position
        p->x = p->prevX;
        p->y = p->prevY;
    }
    else if (p->currentMap && !canMove(p->currentMap, p->x, p->y))
    {
        // Collision detected, revert to previous position
        p->x = p->prevX;
        p->y = p->prevY;
    }
}

static void addSprite(Pacman *p, Direction direction, int spriteX, int spriteY, int width, int height)
{
    Sprite *sprite = NULL;
    switch (direction)
    {
    case LEFT:
        sprite = &p->leftSprite;
        break;
    case UP:
        sprite = &p->upSprite;
        break;
    case RIGHT:
        sprite = &p->rightSprite;
        break;
    case DOWN:
        sprite = &p->downSprite;
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

static Sprite *getSprite(Pacman *p, Direction direction)
{
    switch (direction)
    {
    case LEFT:
        return &p->leftSprite;
    case UP:
        return &p->upSprite;
    case RIGHT:
        return &p->rightSprite;
    case DOWN:
        return &p->downSprite;
    default:
        return NULL; // Invalid direction
    }
}

static void setMap(Pacman *p, UBYTE *map)
{
    p->currentMap = map;
}
