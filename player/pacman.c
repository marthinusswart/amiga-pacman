#include "pacman.h"
#include "ghost/ghost.h"

// Private forward declaration
static void movePacman(Pacman *p, Direction direction);
static void addSprite(Pacman *p, Direction direction, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData);
static short getSprite(Pacman *p, Direction direction, Sprite **sprite_out);
static void setMap(Pacman *p, UBYTE *map);
static int isPacmanColliding(Pacman *p, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost);

short createPacman(Pacman **p_out, int x, int y, int width, int height)
{
    if (!p_out)
        return -1;

    Pacman *p = (Pacman *)AllocMem(sizeof(Pacman), MEMF_CHIP | MEMF_CLEAR);
    if (!p)
        return -1; // Memory allocation failed

    p->x = x;
    p->y = y;
    p->width = width;
    p->height = height;
    p->prevX = x;
    p->prevY = y;
    p->direction = RIGHT;
    p->movePacman = movePacman;               // Assign the function pointer
    p->addSprite = addSprite;                 // Assign the function pointer
    p->getSprite = getSprite;                 // Assign the function pointer
    p->setMap = setMap;                       // Assign the function pointer
    p->isPacmanColliding = isPacmanColliding; // Assign the function pointer
    p->currentMap = NULL;                     // Initialize safely

    *p_out = p; // Assign to the caller's pointer
    return 0;   // Success
}

static void movePacman(Pacman *p, Direction direction)
{
    p->prevX = p->x;
    p->prevY = p->y;
    p->direction = direction;

    short nextX = p->x;
    short nextY = p->y;

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

    if (!isValidSpriteLocation(nextX, nextY, p->width, p->height, 320, 256))
    {
        // Invalid location, do nothing
    }
    else if (p->currentMap)
    {
        short sx = p->x;
        short sy = p->y;
        if (canMove(p->currentMap, &sx, &sy, nextX, nextY))
        {
            p->x = sx;
            p->y = sy;
        }
    }
    else
    {
        p->x = nextX;
        p->y = nextY;
    }
}

static void addSprite(Pacman *p, Direction direction, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData)
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
    sprite->spriteData = spriteTileData; // Example: all sprites use the same data for now
}

static short getSprite(Pacman *p, Direction direction, Sprite **sprite_out)
{
    if (!sprite_out)
        return -1;

    switch (direction)
    {
    case LEFT:
        *sprite_out = &p->leftSprite;
        return 0;
    case UP:
        *sprite_out = &p->upSprite;
        return 0;
    case RIGHT:
        *sprite_out = &p->rightSprite;
        return 0;
    case DOWN:
        *sprite_out = &p->downSprite;
        return 0;
    default:
        *sprite_out = NULL;
        return -1; // Invalid direction
    }
}

static void setMap(Pacman *p, UBYTE *map)
{
    p->currentMap = map;
}

static int isPacmanColliding(Pacman *p, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost)
{
    if (!p)
        return 0;

    if (redGhost && isColliding(p->x, p->y, p->width, p->height, redGhost->x, redGhost->y, redGhost->width, redGhost->height, 5))
        return 1;
    if (blueGhost && isColliding(p->x, p->y, p->width, p->height, blueGhost->x, blueGhost->y, blueGhost->width, blueGhost->height, 5))
        return 1;
    if (pinkGhost && isColliding(p->x, p->y, p->width, p->height, pinkGhost->x, pinkGhost->y, pinkGhost->width, pinkGhost->height, 5))
        return 1;
    if (orangeGhost && isColliding(p->x, p->y, p->width, p->height, orangeGhost->x, orangeGhost->y, orangeGhost->width, orangeGhost->height, 5))
        return 1;

    return 0;
}
