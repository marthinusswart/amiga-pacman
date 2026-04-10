#include "sprite_routines.h"
#include "routines/pathfinding_routines.h"
#include <ace/managers/blit.h>

#define SPRITE_DEBUG_OFF

#ifdef SPRITE_DEBUG_ON
#define SPRITE_PRINTF(...) KPrintF(__VA_ARGS__)
#else
#define SPRITE_PRINTF(...)
#endif

short setupPacman(Pacman **pacman, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Pacman!\n");
    if (createPacman(pacman, 192, 150, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Pacman!\n");
        return -1;
    }
    (*pacman)->setMap(*pacman, mapping_stage_0001);

    /* Setup sprite frame 0 for each direction */
    calculateSpriteLocation(3, 9, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, RIGHT, 0, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, DOWN, 0, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, LEFT, 0, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 11, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, UP, 0, bobX, bobY, 16, 16, spriteTileData);
    /* */

    /* Setup sprite frame 1 (mouth closing) for each direction */
    calculateSpriteLocation(2, 9, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, RIGHT, 1, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, DOWN, 1, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, LEFT, 1, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 11, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, UP, 1, bobX, bobY, 16, 16, spriteTileData);
    /* */

    /* Setup sprite frame 2 (mouth closed) for each direction */
    calculateSpriteLocation(3, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, RIGHT, 2, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, DOWN, 2, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, LEFT, 2, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(3, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, UP, 2, bobX, bobY, 16, 16, spriteTileData);
    /* */

    /* Setup sprite frame 3 (mouth opening) for each direction */
    calculateSpriteLocation(2, 9, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, RIGHT, 3, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, DOWN, 3, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, LEFT, 3, bobX, bobY, 16, 16, spriteTileData);

    calculateSpriteLocation(2, 11, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, UP, 3, bobX, bobY, 16, 16, spriteTileData);
    /* */

    return 0;
}

short setupBlueGhost(Ghost **blueGhost, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(blueGhost, 144, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Blue Ghost!\n");
        return -1;
    }
    (*blueGhost)->setMap(*blueGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, RIGHT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(6, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, DOWN, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(5, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, LEFT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(7, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, UP, bobX, bobY, 16, 16, spriteTileData);

    return 0;
}

short setupRedGhost(Ghost **redGhost, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(redGhost, 128, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Red Ghost!\n");
        return -1;
    }
    (*redGhost)->setMap(*redGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, RIGHT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(6, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, DOWN, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(5, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, LEFT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(7, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, UP, bobX, bobY, 16, 16, spriteTileData);

    return 0;
}

short setupPinkGhost(Ghost **pinkGhost, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(pinkGhost, 160, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Pink Ghost!\n");
        return -1;
    }
    (*pinkGhost)->setMap(*pinkGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, RIGHT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(6, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, DOWN, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(5, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, LEFT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(7, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, UP, bobX, bobY, 16, 16, spriteTileData);

    return 0;
}

short setupOrangeGhost(Ghost **orangeGhost, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(orangeGhost, 176, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Orange Ghost!\n");
        return -1;
    }
    (*orangeGhost)->setMap(*orangeGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, RIGHT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(6, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, DOWN, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(5, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, LEFT, bobX, bobY, 16, 16, spriteTileData);
    calculateSpriteLocation(7, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, UP, bobX, bobY, 16, 16, spriteTileData);

    return 0;
}

short setupStartText(Sprite **startText, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Start Text!\n");
    if (createSprite(startText) != 0)
    {
        SPRITE_PRINTF("Failed to create Start Text!\n");
        return -1;
    }
    calculateSpriteLocation(0, 0, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created START TEXT sprite at (%ld, %ld)\n", bobX, bobY);
    (*startText)->x = bobX;
    (*startText)->y = bobY;
    (*startText)->width = 112;
    (*startText)->height = 16;
    (*startText)->spriteData = spriteTileData;

    return 0;
}

short setupGameOverText(Sprite **gameOverText, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Game Over Text!\n");
    if (createSprite(gameOverText) != 0)
    {
        SPRITE_PRINTF("Failed to create Game Over Text!\n");
        return -1;
    }
    calculateSpriteLocation(0, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created GAME OVER TEXT sprite at (%ld, %ld)\n", bobX, bobY);
    (*gameOverText)->x = bobX;
    (*gameOverText)->y = bobY;
    (*gameOverText)->width = 112;
    (*gameOverText)->height = 16;
    (*gameOverText)->spriteData = spriteTileData;

    return 0;
}

short setupHighScoreText(Sprite **highScoreText, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create High Score Text!\n");
    if (createSprite(highScoreText) != 0)
    {
        SPRITE_PRINTF("Failed to create High Score Text!\n");
        return -1;
    }
    calculateSpriteLocation(1, 0, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created HIGH SCORE TEXT sprite at (%ld, %ld)\n", bobX, bobY);
    (*highScoreText)->x = bobX;
    (*highScoreText)->y = bobY;
    (*highScoreText)->width = 48;
    (*highScoreText)->height = 16;
    (*highScoreText)->spriteData = spriteTileData;

    return 0;
}

short setupPowerPill(Sprite **powerPill, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Power Pill!\n");
    if (createSprite(powerPill) != 0)
    {
        SPRITE_PRINTF("Failed to create Power Pill!\n");
        return -1;
    }
    calculateSpriteLocation(2, 0, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created POWER PILL sprite at (%ld, %ld)\n", bobX, bobY);
    (*powerPill)->x = bobX;
    (*powerPill)->y = bobY;
    (*powerPill)->width = 16;
    (*powerPill)->height = 16;
    (*powerPill)->spriteData = spriteTileData;

    return 0;
}

short setupPellets(Sprite **pellet, const UBYTE *spriteTileData)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Pellet!\n");
    if (createSprite(pellet) != 0)
    {
        SPRITE_PRINTF("Failed to create Pellet!\n");
        return -1;
    }
    calculateSpriteLocation(2, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created PELLET sprite at (%ld, %ld)\n", bobX, bobY);
    (*pellet)->x = bobX;
    (*pellet)->y = bobY;
    (*pellet)->width = 16;
    (*pellet)->height = 16;
    (*pellet)->spriteData = spriteTileData;

    return 0;
}

void ghostUpdates(Pacman *pacman, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost)
{
    // update red ghost pathfinding and move it
    updateChaseGhostDirection(redGhost, pacman);
    redGhost->moveGhost(redGhost, redGhost->direction);

    // update blue ghost pathfinding and move it
    updateUnpredictableGhostDirection(blueGhost, pacman, redGhost);
    blueGhost->moveGhost(blueGhost, blueGhost->direction);

    // update pink ghost pathfinding and move it
    updateAmbushGhostDirection(pinkGhost, pacman);
    pinkGhost->moveGhost(pinkGhost, pinkGhost->direction);

    // update orange ghost pathfinding and move it
    updateCowardGhostDirection(orangeGhost, pacman);
    orangeGhost->moveGhost(orangeGhost, orangeGhost->direction);
}

void updateSpriteMaps(Pacman *pacman, Ghost *blueGhost, Ghost *redGhost, Ghost *pinkGhost,
                      Ghost *orangeGhost, const UBYTE *mapData)
{
    if (pacman && mapData)
        pacman->setMap(pacman, mapData);
    if (blueGhost && mapData)
        blueGhost->setMap(blueGhost, mapData);
    if (redGhost && mapData)
        redGhost->setMap(redGhost, mapData);
    if (pinkGhost && mapData)
        pinkGhost->setMap(pinkGhost, mapData);
    if (orangeGhost && mapData)
        orangeGhost->setMap(orangeGhost, mapData);
}

int addPowerPillsToMap(Sprite *pill, tBitMap *background, tBitMap *pacmanTiles,
                       tBitMap **screenBuffers, const UBYTE *tilesMask, const UBYTE *mapData)
{
    if (!pill || !background || !pacmanTiles || !screenBuffers || !tilesMask || !mapData)
        return -1;

    for (int i = 0; i < 320; i++)
    {
        if (mapData[i] == 2) // 2 = Power Pill
        {
            int tileX = (i % 20) * 16;
            int tileY = (i / 20) * 16;

            // Add to the permanent background
            blitCopyMask(
                pacmanTiles, pill->x, pill->y,
                background, tileX, tileY,
                pill->width, pill->height,
                tilesMask);

            // Add to the front and back screen buffers to make it immediately visible
            blitCopyMask(
                pacmanTiles, pill->x, pill->y,
                screenBuffers[0], tileX, tileY,
                pill->width, pill->height,
                tilesMask);
            blitCopyMask(
                pacmanTiles, pill->x, pill->y,
                screenBuffers[1], tileX, tileY,
                pill->width, pill->height,
                tilesMask);
        }
    }

    return 0;
}

int addPelletsToMap(Sprite *pellet, UBYTE *pelletsOnMap, tBitMap *background, tBitMap *pacmanTiles,
                    tBitMap **screenBuffers, const UBYTE *tilesMask, const UBYTE *mapData)
{
    if (!pellet || !pelletsOnMap || !background || !pacmanTiles || !screenBuffers || !tilesMask || !mapData)
        return -1;

    for (int i = 0; i < 320; i++)
    {
        pelletsOnMap[i] = 0; // Clear pellet state
        if (mapData[i] == 0) // 0 = Path (Pellet)
        {
            pelletsOnMap[i] = 1; // Mark pellet as present on the map
            int tileX = (i % 20) * 16;
            int tileY = (i / 20) * 16;

            // Add to the permanent background
            blitCopyMask(
                pacmanTiles, pellet->x, pellet->y,
                background, tileX, tileY,
                pellet->width, pellet->height,
                tilesMask);

            // Add to the front and back screen buffers to make it immediately visible
            blitCopyMask(
                pacmanTiles, pellet->x, pellet->y,
                screenBuffers[0], tileX, tileY,
                pellet->width, pellet->height,
                tilesMask);
            blitCopyMask(
                pacmanTiles, pellet->x, pellet->y,
                screenBuffers[1], tileX, tileY,
                pellet->width, pellet->height,
                tilesMask);
        }
    }

    return 0;
}

short setupSprite(Sprite **sprite, const UBYTE *spriteTileData, int row, int col,
                  int width, int height, int tilesetWidth, int tilesetHeight)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Setting up Sprite!\n");
    if (createSprite(sprite) != 0)
    {
        SPRITE_PRINTF("Failed to create Sprite!\n");
        return -1;
    }
    calculateSpriteLocation(row, col, width, height, tilesetWidth, tilesetHeight, &bobX, &bobY);
    SPRITE_PRINTF("Created Sprite at (%ld, %ld)\n", bobX, bobY);
    (*sprite)->x = bobX;
    (*sprite)->y = bobY;
    (*sprite)->width = width;
    (*sprite)->height = height;
    (*sprite)->spriteData = spriteTileData;

    return 0;
}
