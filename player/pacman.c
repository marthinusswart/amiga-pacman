#include "pacman.h"
#include "ghost/ghost.h"

#define PACMAN_DEBUG_OFF

#define MOUTH_OPEN 0;
#define MOUTH_HALF_CLOSING 1;
#define MOUTH_CLOSED 2;
#define MOUTH_HALF_OPENING 3;

// Private forward declaration
static void movePacman(Pacman *p, Direction direction);
static void addSprite(Pacman *p, Direction direction, int index, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData);
static short getSprite(Pacman *p, Direction direction, Sprite **sprite_out);
static short getSpriteIdx(Pacman *p, Direction direction, int index, Sprite **sprite_out);
static void setMap(Pacman *p, const UBYTE *map);
static int isPacmanColliding(Pacman *p, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost);
static void updateAnimation(Pacman *p, Direction direction);
static void pulseCheck(Pacman *p);

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
    p->speed = 1; // Default speed, can be modified later
    p->currentAnimationIndex = 0;
    p->animationCounter = 0;
    p->isAnimated = TRUE;
    p->wasMoving = FALSE;
    p->direction = RIGHT;
    p->lastDirection = RIGHT;
    p->movePacman = movePacman;               // Assign the function pointer
    p->addSprite = addSprite;                 // Assign the function pointer
    p->getSprite = getSprite;                 // Assign the function pointer
    p->getSpriteIdx = getSpriteIdx;           // Assign the function pointer
    p->setMap = setMap;                       // Assign the function pointer
    p->isPacmanColliding = isPacmanColliding; // Assign the function pointer
    p->pulseCheck = pulseCheck;               // Assign the function pointer
    p->currentMap = NULL;                     // Initialize safely

    *p_out = p; // Assign to the caller's pointer
    return 0;   // Success
}

static void movePacman(Pacman *p, Direction direction)
{
    p->prevX = p->x;
    p->prevY = p->y;
    p->direction = direction;
    p->wasMoving = TRUE;

    updateAnimation(p, direction);

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

static void updateAnimation(Pacman *p, Direction direction)
{
    if (p->isAnimated)
    {
        if (direction != p->lastDirection)
        {
            p->currentAnimationIndex = 0; // Reset animation when changing direction
            p->animationCounter = 0;
            p->lastDirection = direction;
        }
        else
        {
            if (p->animationCounter < (MAX_PACMAN_ANIMATION_SPEED - p->speed)) // Adjust this value to control animation speed
            {
                p->animationCounter++;
            }
            else
            {
                p->animationCounter = 0; // Reset counter for next frame
                // Cycle through animation frames for the current direction
                if (p->currentAnimationIndex >= (PACMAN_ANIMATION_FRAMES - 1))
                {
                    p->currentAnimationIndex = 0;
                }
                else if (p->currentAnimationIndex <= (PACMAN_ANIMATION_FRAMES - 1))
                {
                    p->currentAnimationIndex = (p->currentAnimationIndex + 1);
                }

#ifdef PACMAN_DEBUG_ON
                DEBUG_PRINT("Pacman animation updated: direction=%ld, currentAnimationIndex=%ld\n", direction, p->currentAnimationIndex);
#endif
            }
        }
    }
}

static void addSprite(Pacman *p, Direction direction, int index, int spriteX, int spriteY, int width, int height, const UBYTE *spriteTileData)
{
    if (index < 0 || index >= PACMAN_ANIMATION_FRAMES)
        return; // Prevent out of bounds index

    Sprite *sprite = NULL;
    switch (direction)
    {
    case LEFT:
        sprite = &p->leftSprites[index];
        break;
    case UP:
        sprite = &p->upSprites[index];
        break;
    case RIGHT:
        sprite = &p->rightSprites[index];
        break;
    case DOWN:
        sprite = &p->downSprites[index];
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
    return getSpriteIdx(p, direction, p->currentAnimationIndex, sprite_out);
}

static short getSpriteIdx(Pacman *p, Direction direction, int index, Sprite **sprite_out)
{
    if (!sprite_out || index < 0 || index >= PACMAN_ANIMATION_FRAMES)
        return -1;

    switch (direction)
    {
    case LEFT:
        *sprite_out = &p->leftSprites[index];
        return 0;
    case UP:
        *sprite_out = &p->upSprites[index];
        return 0;
    case RIGHT:
        *sprite_out = &p->rightSprites[index];
        return 0;
    case DOWN:
        *sprite_out = &p->downSprites[index];
        return 0;
    default:
        *sprite_out = NULL;
        return -1; // Invalid direction
    }
}

static void setMap(Pacman *p, const UBYTE *map)
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

static void pulseCheck(Pacman *p)
{
    if (p->wasMoving)
    {
        p->wasMoving = FALSE; // Reset the flag for the next update
    }
    else
    {
        // Pacman is not moving, reset animation to the first frame (mouth open)
        p->currentAnimationIndex = 0;
        p->animationCounter = 0;
    }
}
