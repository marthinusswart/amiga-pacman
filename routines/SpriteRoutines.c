#include "SpriteRoutines.h"
#include "player/pacman.h"
#include "ghost/ghost.h"
#include "routines/ScreenRoutines.h"
#include "map/Stages.h"
#include "support/gcc8_c_support.h"

#define SPRITE_DEBUG_OFF

#ifdef SPRITE_DEBUG
#define SPRITE_PRINTF(...) KPrintF(__VA_ARGS__)
#else
#define SPRITE_PRINTF(...)
#endif

extern Pacman *pacman;
extern Ghost *blueGhost;
extern Ghost *redGhost;
extern Ghost *pinkGhost;
extern Ghost *orangeGhost;

void setupPacman(void)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Pacman!\n");
    if (createPacman(&pacman, 192, 150, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Pacman!\n");
        return;
    }
    pacman->setMap(pacman, mapping_stage_0001);
    calculateSpriteLocation(3, 9, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    pacman->addSprite(pacman, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    pacman->addSprite(pacman, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    pacman->addSprite(pacman, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(3, 11, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    pacman->addSprite(pacman, UP, bobX, bobY, 16, 16);
}

void setupBlueGhost(void)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(&blueGhost, 144, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Blue Ghost!\n");
        return;
    }
    blueGhost->setMap(blueGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    blueGhost->addSprite(blueGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    blueGhost->addSprite(blueGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    blueGhost->addSprite(blueGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 1, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    blueGhost->addSprite(blueGhost, UP, bobX, bobY, 16, 16);
}

void setupRedGhost(void)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(&redGhost, 128, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Red Ghost!\n");
        return;
    }
    redGhost->setMap(redGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    redGhost->addSprite(redGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    redGhost->addSprite(redGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    redGhost->addSprite(redGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 7, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    redGhost->addSprite(redGhost, UP, bobX, bobY, 16, 16);
}

void setupPinkGhost(void)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(&pinkGhost, 160, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Pink Ghost!\n");
        return;
    }
    pinkGhost->setMap(pinkGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    pinkGhost->addSprite(pinkGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    pinkGhost->addSprite(pinkGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    pinkGhost->addSprite(pinkGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 5, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    pinkGhost->addSprite(pinkGhost, UP, bobX, bobY, 16, 16);
}

void setupOrangeGhost(void)
{
    int bobX = 0;
    int bobY = 0;
    SPRITE_PRINTF("Create Ghost!\n");
    if (createGhost(&orangeGhost, 176, 96, 16, 16) != 0)
    {
        SPRITE_PRINTF("Failed to create Orange Ghost!\n");
        return;
    }
    orangeGhost->setMap(orangeGhost, mapping_stage_0001);
    calculateSpriteLocation(4, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
    orangeGhost->addSprite(orangeGhost, RIGHT, bobX, bobY, 16, 16);
    calculateSpriteLocation(6, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
    orangeGhost->addSprite(orangeGhost, DOWN, bobX, bobY, 16, 16);
    calculateSpriteLocation(5, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
    orangeGhost->addSprite(orangeGhost, LEFT, bobX, bobY, 16, 16);
    calculateSpriteLocation(7, 3, 16, 16, 320, 320, &bobX, &bobY);
    SPRITE_PRINTF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
    orangeGhost->addSprite(orangeGhost, UP, bobX, bobY, 16, 16);
}