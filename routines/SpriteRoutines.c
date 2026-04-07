#include "SpriteRoutines.h"

#define SPRITE_DEBUG_OFF

#ifdef SPRITE_DEBUG
#define SPRITE_PRINTF(...) KPrintF(__VA_ARGS__)
#else
#define SPRITE_PRINTF(...)
#endif

short setupPacman(Pacman **pacman)
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
    calculateSpriteLocation(3, 9, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 11, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pacman)->addSprite(*pacman, UP, bobX, bobY, 16, 16);

    return 0;
}

short setupBlueGhost(Ghost **blueGhost)
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
    (*blueGhost)->addSprite(*blueGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*blueGhost)->addSprite(*blueGhost, UP, bobX, bobY, 16, 16);

    return 0;
}

short setupRedGhost(Ghost **redGhost)
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
    (*redGhost)->addSprite(*redGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*redGhost)->addSprite(*redGhost, UP, bobX, bobY, 16, 16);

    return 0;
}

short setupPinkGhost(Ghost **pinkGhost)
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
    (*pinkGhost)->addSprite(*pinkGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*pinkGhost)->addSprite(*pinkGhost, UP, bobX, bobY, 16, 16);

    return 0;
}

short setupOrangeGhost(Ghost **orangeGhost)
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
    (*orangeGhost)->addSprite(*orangeGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    (*orangeGhost)->addSprite(*orangeGhost, UP, bobX, bobY, 16, 16);

    return 0;
}

short setupStartText(Sprite **startText)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Start Text!\n");
    if (createSprite(startText) != 0)
    {
        SPRITE_PRINTF("Failed to create Start Text!\n");
        return -1;
    }
    calculateSpriteLocation(0, 0, 112, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created START TEXT sprite at (%ld, %ld)\n", bobX, bobY);
    (*startText)->x = bobX;
    (*startText)->y = bobY;
    (*startText)->width = 112;
    (*startText)->height = 16;
    (*startText)->spriteData = (const UBYTE *)pacman_tiles2; // Example: all sprites use the same data for now

    return 0;
}