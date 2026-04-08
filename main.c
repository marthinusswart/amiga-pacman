#include <ace/managers/system.h>
#include <ace/managers/key.h>
#include <ace/utils/bitmap.h>
#include <ace/managers/blit.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <exec/execbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <ace/utils/custom.h>
#include <hardware/intbits.h>
#include "support/gcc8_c_support.h"
#include "routines/screen_routines.h"
#include "routines/system_routines.h"
#include "routines/mouse_routines.h"
#include "routines/music_routines.h"
#include "routines/copper_routines.h"
#include "routines/collision_routines.h"
#include "routines/pathfinding_routines.h"
#include "routines/sprite_routines.h"
#include "player/pacman.h"
#include "ghost/ghost.h"
#include "routines/state_routines.h"
#include "state/state_ext.h"

// config
#define MUSIC_OFF

// forward declarations
static void resetGameState(void);
static void addPelletsToMap(Sprite *pellet);

struct ExecBase *SysBase;
volatile struct Custom *custom;
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;
Pacman *pacman;
Ghost *blueGhost;
Ghost *redGhost;
Ghost *pinkGhost;
Ghost *orangeGhost;
Sprite *startText;
Sprite *gameOverText;
Sprite *powerPill;
Sprite *pellet;

// backup
UWORD SystemInts;
UWORD SystemDMA;
UWORD SystemADKCON;
volatile APTR VBR = 0;
APTR SystemIrq;

struct View *ActiView;

tBitMap *tScreenBuffers[2] = {NULL, NULL};
USHORT *bplPtrsInCopper = NULL;
int frontBufferIdx = 0;
int backBufferIdx = 1;
tBitMap *tPacmanTiles = NULL;
tBitMap *tBackground = NULL;

// Double buffering history trackers. Array index 0 tracks Buffer 0, index 1 tracks Buffer 1.
static Position lastPosition[NUM_ENTITIES][2];

static UBYTE pelletsOnMap[320];

// DEMO - INCBIN
volatile short frameCounter = 0;
INCBIN(colors, "pal/pacman_tiles.pal")
INCBIN_CHIP(pacman_tiles, "bpl/pacman_tiles.bpl")
INCBIN_CHIP(pacman_tiles_mask, "bpl/pacman_tiles_mask.bpl")
INCBIN_CHIP(pacman_stage_01, "bpl/stage-0001.bpl")

static void vblankHandler(volatile struct Custom *pCustom, volatile void *pData)
{
#ifdef MUSIC
	// DEMO - ThePlayer
	p61Music();
#endif
	// DEMO - increment frameCounter
	frameCounter++;
}

static void Wait10() { WaitLine(0x10); }
static void Wait11() { WaitLine(0x11); }
static void Wait12() { WaitLine(0x12); }
static void Wait13() { WaitLine(0x13); }

static void setupEnvironment(void)
{
	SysBase = *((struct ExecBase **)4UL);
	custom = (struct Custom *)0xdff000;

	DOSBase = (struct DosLibrary *)OpenLibrary((CONST_STRPTR) "dos.library", 0);
	GfxBase = (struct GfxBase *)OpenLibrary((CONST_STRPTR) "graphics.library", 0);

	KPrintF("Hello debugger from Amiga!\n");

	if (DOSBase)
	{
		Write(Output(), (APTR) "Hello console!\n", 15);
		Delay(50); // This requires the OS timer.device to be alive!
	}

	// ACE's system.c comments claim to save ADKCON, but actually forgets to!
	// We must back it up manually to ensure disk drives and audio return to normal.
	SystemADKCON = custom->adkconr;
	SystemInts = custom->intenar;
	SystemDMA = custom->dmaconr;

	systemCreate(); // Seize the system BEFORE p61Init pollutes the hardware registers
	systemUnuse();	// Fully suspend the OS so it stops overwriting the copper list

	warpmode(1);
}

static void teardownEnvironment(void)
{
	KPrintF("Destroying System!\n");
	if (tScreenBuffers[0])
		bitmapDestroy(tScreenBuffers[0]);
	if (tScreenBuffers[1])
		bitmapDestroy(tScreenBuffers[1]);
	if (tPacmanTiles)
		FreeMem(tPacmanTiles, sizeof(tBitMap));
	if (tBackground)
		FreeMem(tBackground, sizeof(tBitMap));

	systemDestroy();

	// MANUALLY RESTORE STATE (Ignoring whatever ACE missed/messed up)
	// ACE uses LoadView() which waits for the OS, but the OS might be too slow
	// causing our FreeMem to rip the Copper list out from under the hardware!
	Disable();
	custom->intena = 0x7FFF; // Disable all
	custom->intreq = 0x7FFF; // Clear pending
	custom->dmacon = 0x7FFF; // Disable all DMA

	// Forcefully point the hardware back to the OS Copper lists INSTANTLY
	custom->cop1lc = (ULONG)GfxBase->copinit;
	custom->cop2lc = (ULONG)GfxBase->LOFlist;
	custom->copjmp1 = 0x7fff;

	custom->intena = SystemInts | 0x8000;
	custom->dmacon = SystemDMA | 0x8000;
	custom->adkcon = SystemADKCON | 0x8000;
	Enable();

	if (GfxBase)
	{
		KPrintF("free gfx library!\n");
		CloseLibrary((struct Library *)GfxBase);
	}

	if (DOSBase)
	{
		KPrintF("free dos library!\n");
		CloseLibrary((struct Library *)DOSBase);
	}
}

static USHORT *setupCopper(void)
{
	USHORT *copper1 = (USHORT *)AllocMem(1024, MEMF_CHIP);
	USHORT *copPtr = copper1;

	// 1. Create PLANAR screen buffers safely using ACE (BMF_DISPLAYABLE forces CHIP RAM)
	tScreenBuffers[0] = bitmapCreate(320, 256, 5, BMF_CLEAR | BMF_DISPLAYABLE);
	tScreenBuffers[1] = bitmapCreate(320, 256, 5, BMF_CLEAR | BMF_DISPLAYABLE);

	// 2. Wrap the INCBIN planar tile data directly in a tBitMap (no memory copy needed!)
	tPacmanTiles = (tBitMap *)AllocMem(sizeof(tBitMap), MEMF_PUBLIC | MEMF_CLEAR);
	InitBitMap((struct BitMap *)tPacmanTiles, 5, 320, 320); // Assumes tileset is 320x320
	for (int p = 0; p < 5; p++)
	{
		tPacmanTiles->Planes[p] = (PLANEPTR)(pacman_tiles + p * (320 / 8) * 320);
	}

	// 3. Wrap the background image data in a tBitMap
	tBackground = (tBitMap *)AllocMem(sizeof(tBitMap), MEMF_PUBLIC | MEMF_CLEAR);
	InitBitMap((struct BitMap *)tBackground, 5, 320, 256);
	for (int p = 0; p < 5; p++)
	{
		tBackground->Planes[p] = (PLANEPTR)(pacman_stage_01 + p * (320 / 8) * 256);
	}

	// Copy the background into the screen buffer initially
	for (int p = 0; p < 5; p++)
	{
		CopyMem(tBackground->Planes[p], tScreenBuffers[0]->Planes[p], (320 / 8) * 256);
		CopyMem(tBackground->Planes[p], tScreenBuffers[1]->Planes[p], (320 / 8) * 256);
	}

	screenScanDefault(&copPtr);

	// enable bitplanes
	*copPtr++ = offsetof(struct Custom, bplcon0);
	*copPtr++ = (0 << 10) /*dual pf*/ | (1 << 9) /*color*/ | ((5) << 12) /*num bitplanes*/;
	*copPtr++ = offsetof(struct Custom, bplcon1); // scrolling
	*copPtr++ = 0;
	*copPtr++ = offsetof(struct Custom, bplcon2); // playfied priority
	*copPtr++ = 1 << 6;							  // 0x24;			//Sprites have priority over playfields

	// set bitplane modulo for PLANAR
	*copPtr++ = offsetof(struct Custom, bpl1mod); // odd planes   1,3,5
	*copPtr++ = 0;
	*copPtr++ = offsetof(struct Custom, bpl2mod); // even  planes 2,4
	*copPtr++ = 0;

	// set bitplane pointers
	const UBYTE *planes[5];
	for (int a = 0; a < 5; a++)
	{
		planes[a] = tScreenBuffers[0]->Planes[a];
	}
	bplPtrsInCopper = copPtr;			 // Save this globally so we can update it in the main loop!
	copSetPlanes(0, &copPtr, planes, 5); // INJECT pointers into copper list!

	// set colors
	for (int a = 0; a < 32; a++)
		copSetColor(&copPtr, a, ((USHORT *)colors)[a]);

	// end copper list
	*copPtr++ = 0xffff;
	*copPtr++ = 0xfffe;

	custom->cop1lc = (ULONG)copper1;
	custom->dmacon = DMAF_BLITTER; // disable blitter dma for copjmp bug
	custom->copjmp1 = 0x7fff;	   // start coppper

	return copper1;
}

static int processInputs(void)
{
	static UBYTE prevSpaceState = 0;
	UBYTE currSpaceState = keyCheck(KEY_SPACE);

	if (keyCheck(KEY_ESCAPE))
		return 0; // Signal to break the main loop

	if (getGameState(PLAYING_STATE) == ON)
	{
		if (keyCheck(KEY_LEFT) || keyCheck(KEY_A))
			pacman->movePacman(pacman, LEFT);
		else if (keyCheck(KEY_RIGHT) || keyCheck(KEY_D))
			pacman->movePacman(pacman, RIGHT);
		else if (keyCheck(KEY_UP) || keyCheck(KEY_W))
			pacman->movePacman(pacman, UP);
		else if (keyCheck(KEY_DOWN) || keyCheck(KEY_S))
			pacman->movePacman(pacman, DOWN);
	}

	else if (currSpaceState && !prevSpaceState)
	{
		if (getGameState(GAME_OVER_TEXT) == ON)
		{
			KPrintF("Resetting game state!\n");
			resetGameState();
		}
		else
		{
			KPrintF("Starting game state!\n");
			updateGameState(START_GAME_TEXT, OFF);
			updateGameState(PLAYING_STATE, ON);
		}
	}

	prevSpaceState = currSpaceState;

	return 1; // Signal to continue
}

static void ghostUpdates(void)
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

static void gameStartUpdates(void)
{
	if (getGameState(START_GAME_TEXT) == ON)
	{
		blitCopyMask(
			tPacmanTiles, startText->x, startText->y,
			tScreenBuffers[backBufferIdx], 108, 112,
			startText->width, startText->height,
			(const UBYTE *)pacman_tiles_mask);
	}
	else if (getGameState(CLEARED_START_TEXT) == OFF)
	{
		updateGameState(CLEARED_START_TEXT, ON);
		// Erase the start text by copying the background over it
		blitCopy(
			tBackground, 108, 112,
			tScreenBuffers[0], 108, 112,
			startText->width, startText->height, MINTERM_COOKIE);

		blitCopy(
			tBackground, 108, 112,
			tScreenBuffers[1], 108, 112,
			startText->width, startText->height, MINTERM_COOKIE);
	}

	if (getGameState(CLEAR_GAME_OVER_TEXT) == ON)
	{
		updateGameState(CLEAR_GAME_OVER_TEXT, OFF);
		// Erase the start text by copying the background over it
		blitCopy(
			tBackground, 108, 16,
			tScreenBuffers[0], 108, 16,
			gameOverText->width, gameOverText->height, MINTERM_COOKIE);

		blitCopy(
			tBackground, 108, 16,
			tScreenBuffers[1], 108, 16,
			gameOverText->width, gameOverText->height, MINTERM_COOKIE);
	}
}

static void backgroundUpdates(void)
{
	blitCopy(tBackground, lastPosition[ORANGE][backBufferIdx].x, lastPosition[ORANGE][backBufferIdx].y,
			 tScreenBuffers[backBufferIdx], lastPosition[ORANGE][backBufferIdx].x, lastPosition[ORANGE][backBufferIdx].y,
			 orangeGhost->width, orangeGhost->height, MINTERM_COOKIE);

	blitCopy(tBackground, lastPosition[BLUE][backBufferIdx].x, lastPosition[BLUE][backBufferIdx].y,
			 tScreenBuffers[backBufferIdx], lastPosition[BLUE][backBufferIdx].x, lastPosition[BLUE][backBufferIdx].y,
			 blueGhost->width, blueGhost->height, MINTERM_COOKIE);

	blitCopy(tBackground, lastPosition[RED][backBufferIdx].x, lastPosition[RED][backBufferIdx].y,
			 tScreenBuffers[backBufferIdx], lastPosition[RED][backBufferIdx].x, lastPosition[RED][backBufferIdx].y,
			 redGhost->width, redGhost->height, MINTERM_COOKIE);

	blitCopy(tBackground, lastPosition[PINK][backBufferIdx].x, lastPosition[PINK][backBufferIdx].y,
			 tScreenBuffers[backBufferIdx], lastPosition[PINK][backBufferIdx].x, lastPosition[PINK][backBufferIdx].y,
			 pinkGhost->width, pinkGhost->height, MINTERM_COOKIE);

	blitCopy(tBackground, lastPosition[PACMAN][backBufferIdx].x, lastPosition[PACMAN][backBufferIdx].y,
			 tScreenBuffers[backBufferIdx], lastPosition[PACMAN][backBufferIdx].x, lastPosition[PACMAN][backBufferIdx].y,
			 pacman->width, pacman->height, MINTERM_COOKIE);
}

static void bobUpdates(void)
{
	Sprite *blueSprite = NULL;
	blueGhost->getSprite(blueGhost, blueGhost->direction, &blueSprite);
	Sprite *redSprite = NULL;
	redGhost->getSprite(redGhost, redGhost->direction, &redSprite);
	Sprite *pinkSprite = NULL;
	pinkGhost->getSprite(pinkGhost, pinkGhost->direction, &pinkSprite);
	Sprite *orangeSprite = NULL;
	orangeGhost->getSprite(orangeGhost, orangeGhost->direction, &orangeSprite);
	Sprite *pacSprite = NULL;
	pacman->getSprite(pacman, pacman->direction, &pacSprite);

	// Save the locations we are about to draw to, so we can erase them next time this buffer is active
	lastPosition[BLUE][backBufferIdx].x = blueGhost->x;
	lastPosition[BLUE][backBufferIdx].y = blueGhost->y;

	lastPosition[RED][backBufferIdx].x = redGhost->x;
	lastPosition[RED][backBufferIdx].y = redGhost->y;

	lastPosition[PINK][backBufferIdx].x = pinkGhost->x;
	lastPosition[PINK][backBufferIdx].y = pinkGhost->y;

	lastPosition[ORANGE][backBufferIdx].x = orangeGhost->x;
	lastPosition[ORANGE][backBufferIdx].y = orangeGhost->y;

	lastPosition[PACMAN][backBufferIdx].x = pacman->x;
	lastPosition[PACMAN][backBufferIdx].y = pacman->y;

	if (orangeSprite)
		blitCopyMask(
			tPacmanTiles, orangeSprite->x, orangeSprite->y,
			tScreenBuffers[backBufferIdx], orangeGhost->x, orangeGhost->y,
			orangeGhost->width, orangeGhost->height,
			(const UBYTE *)pacman_tiles_mask);

	if (blueSprite)
		blitCopyMask(
			tPacmanTiles, blueSprite->x, blueSprite->y,
			tScreenBuffers[backBufferIdx], blueGhost->x, blueGhost->y,
			blueGhost->width, blueGhost->height,
			(const UBYTE *)pacman_tiles_mask);

	if (redSprite)
		blitCopyMask(
			tPacmanTiles, redSprite->x, redSprite->y,
			tScreenBuffers[backBufferIdx], redGhost->x, redGhost->y,
			redGhost->width, redGhost->height,
			(const UBYTE *)pacman_tiles_mask);

	if (pinkSprite)
		blitCopyMask(
			tPacmanTiles, pinkSprite->x, pinkSprite->y,
			tScreenBuffers[backBufferIdx], pinkGhost->x, pinkGhost->y,
			pinkGhost->width, pinkGhost->height,
			(const UBYTE *)pacman_tiles_mask);

	if (pacSprite)
		blitCopyMask(
			tPacmanTiles, pacSprite->x, pacSprite->y,
			tScreenBuffers[backBufferIdx], pacman->x, pacman->y,
			pacman->width, pacman->height,
			(const UBYTE *)pacman_tiles_mask);
}

static void doubleBufferUpdates(void)
{
	const UBYTE *planes[5];
	for (int a = 0; a < 5; a++)
	{
		planes[a] = tScreenBuffers[backBufferIdx]->Planes[a];
	}
	// Safely swap the bitplane pointers in the copper list
	USHORT *tempBplPtr = bplPtrsInCopper;
	copSetPlanes(0, &tempBplPtr, planes, 5);

	// Flip buffers for the next frame
	frontBufferIdx = backBufferIdx;
	backBufferIdx = 1 - frontBufferIdx;
}

static void setGameOverState(void)
{
	updateGameState(START_GAME_TEXT, OFF);
	updateGameState(GAME_OVER_TEXT, ON);
	updateGameState(PLAYING_STATE, OFF);
}

static void resetGameState(void)
{

	if (pacman)
	{
		pacman->x = 192;
		pacman->y = 150;
		pacman->direction = RIGHT;
	}
	if (blueGhost)
	{
		blueGhost->x = 144;
		blueGhost->y = 96;
		blueGhost->direction = RIGHT;
	}
	if (redGhost)
	{
		redGhost->x = 128;
		redGhost->y = 96;
		redGhost->direction = RIGHT;
	}
	if (pinkGhost)
	{
		pinkGhost->x = 160;
		pinkGhost->y = 96;
		pinkGhost->direction = RIGHT;
	}
	if (orangeGhost)
	{
		orangeGhost->x = 176;
		orangeGhost->y = 96;
		orangeGhost->direction = RIGHT;
	}

	addPelletsToMap(pellet);

	updateGameState(START_GAME_TEXT, ON);
	updateGameState(CLEARED_START_TEXT, OFF);
	updateGameState(PLAYING_STATE, OFF);
	updateGameState(GAME_OVER_TEXT, OFF);
	updateGameState(CLEAR_GAME_OVER_TEXT, ON);
}

static int pacmanCollide(Pacman *pacman, Ghost *redGhost, Ghost *blueGhost, Ghost *pinkGhost, Ghost *orangeGhost)
{
	if (!pacman)
		return 0;

	if (redGhost && isColliding(pacman->x, pacman->y, pacman->width, pacman->height, redGhost->x, redGhost->y, redGhost->width, redGhost->height, 5))
		return 1;
	if (blueGhost && isColliding(pacman->x, pacman->y, pacman->width, pacman->height, blueGhost->x, blueGhost->y, blueGhost->width, blueGhost->height, 5))
		return 1;
	if (pinkGhost && isColliding(pacman->x, pacman->y, pacman->width, pacman->height, pinkGhost->x, pinkGhost->y, pinkGhost->width, pinkGhost->height, 5))
		return 1;
	if (orangeGhost && isColliding(pacman->x, pacman->y, pacman->width, pacman->height, orangeGhost->x, orangeGhost->y, orangeGhost->width, orangeGhost->height, 5))
		return 1;

	return 0;
}

static void displayGameOverText(void)
{
	blitCopyMask(
		tPacmanTiles, gameOverText->x, gameOverText->y,
		tScreenBuffers[0], 108, 16,
		gameOverText->width, gameOverText->height,
		(const UBYTE *)pacman_tiles_mask);
	blitCopyMask(
		tPacmanTiles, gameOverText->x, gameOverText->y,
		tScreenBuffers[1], 108, 16,
		gameOverText->width, gameOverText->height,
		(const UBYTE *)pacman_tiles_mask);
}

static void addPowerPillsToMap(Sprite *pill)
{
	if (!pill || !tBackground || !tPacmanTiles)
		return;

	for (int i = 0; i < 320; i++)
	{
		if (mapping_stage_0001[i] == 2) // 2 = Power Pill
		{
			int tileX = (i % 20) * 16;
			int tileY = (i / 20) * 16;

			// Add to the permanent background
			blitCopyMask(
				tPacmanTiles, pill->x, pill->y,
				tBackground, tileX, tileY,
				pill->width, pill->height,
				(const UBYTE *)pacman_tiles_mask);

			// Add to the front and back screen buffers to make it immediately visible
			blitCopyMask(
				tPacmanTiles, pill->x, pill->y,
				tScreenBuffers[0], tileX, tileY,
				pill->width, pill->height,
				(const UBYTE *)pacman_tiles_mask);
			blitCopyMask(
				tPacmanTiles, pill->x, pill->y,
				tScreenBuffers[1], tileX, tileY,
				pill->width, pill->height,
				(const UBYTE *)pacman_tiles_mask);
		}
	}
}

static void addPelletsToMap(Sprite *pellet)
{
	if (!pellet || !tBackground || !tPacmanTiles)
		return;

	for (int i = 0; i < 320; i++)
	{
		pelletsOnMap[i] = 0;			// Clear pellet state
		if (mapping_stage_0001[i] == 0) // 0 = Path (Pellet)
		{
			pelletsOnMap[i] = 1; // Mark pellet as present on the map
			int tileX = (i % 20) * 16;
			int tileY = (i / 20) * 16;

			// Add to the permanent background
			blitCopyMask(
				tPacmanTiles, pellet->x, pellet->y,
				tBackground, tileX, tileY,
				pellet->width, pellet->height,
				(const UBYTE *)pacman_tiles_mask);

			// Add to the front and back screen buffers to make it immediately visible
			blitCopyMask(
				tPacmanTiles, pellet->x, pellet->y,
				tScreenBuffers[0], tileX, tileY,
				pellet->width, pellet->height,
				(const UBYTE *)pacman_tiles_mask);
			blitCopyMask(
				tPacmanTiles, pellet->x, pellet->y,
				tScreenBuffers[1], tileX, tileY,
				pellet->width, pellet->height,
				(const UBYTE *)pacman_tiles_mask);
		}
	}
}

static void updatePellets(Pacman *pacman, UBYTE *pelletsOnMap, tBitMap *tBackground, tBitMap *frontBuffer, tBitMap *backBuffer)
{
	if (!pacman || !pelletsOnMap || !tBackground || !frontBuffer || !backBuffer)
		return;

	// Calculate the center of Pac-Man to determine which tile he is currently eating
	int centerX = pacman->x + (pacman->width / 2);
	int centerY = pacman->y + (pacman->height / 2);

	if (tileHasPellet(pelletsOnMap, centerX, centerY))
	{
		int tileCol = centerX >> 4; // divide by 16
		int tileRow = centerY >> 4; // divide by 16
		int tileIndex = tileRow * 20 + tileCol;

		pelletsOnMap[tileIndex] = 0;
		KPrintF("Pellet picked up!\n");

		int tileX = tileCol * 16;
		int tileY = tileRow * 16;

		// Erase the pellet by filling its 16x16 bounding box with color 0 (black)
		blitRect(tBackground, tileX, tileY, 16, 16, 0);
	}
}

static void setupSprites(void)
{
	setupPacman(&pacman);
	setupBlueGhost(&blueGhost);
	setupRedGhost(&redGhost);
	setupPinkGhost(&pinkGhost);
	setupOrangeGhost(&orangeGhost);
	setupStartText(&startText);
	setupGameOverText(&gameOverText);
	setupPowerPill(&powerPill);
	setupPellets(&pellet);
}

int main()
{
	setupEnvironment();
	// TODO: precalc stuff here
#ifdef MUSIC
	if (p61Init(module) != 0)
		KPrintF("p61Init failed!\n");
#endif
	warpmode(0);
	WaitVbl();

	USHORT *copper1 = setupCopper();
	initializeGameState();

	setupSprites();

	// Initialize double buffering history trackers
	lastPosition[BLUE][0].x = lastPosition[BLUE][1].x = blueGhost->x;
	lastPosition[BLUE][0].y = lastPosition[BLUE][1].y = blueGhost->y;

	lastPosition[RED][0].x = lastPosition[RED][1].x = redGhost->x;
	lastPosition[RED][0].y = lastPosition[RED][1].y = redGhost->y;

	lastPosition[PINK][0].x = lastPosition[PINK][1].x = pinkGhost->x;
	lastPosition[PINK][0].y = lastPosition[PINK][1].y = pinkGhost->y;

	lastPosition[ORANGE][0].x = lastPosition[ORANGE][1].x = orangeGhost->x;
	lastPosition[ORANGE][0].y = lastPosition[ORANGE][1].y = orangeGhost->y;

	lastPosition[PACMAN][0].x = lastPosition[PACMAN][1].x = pacman->x;
	lastPosition[PACMAN][0].y = lastPosition[PACMAN][1].y = pacman->y;

	systemSetDmaMask(DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER, 1); // Tell ACE to enable DMA

	// Before game loop starts, we must set our custom interrupt handler to ensure music and copper list run during the menu screen and gameplay!
	systemSetInt(INTB_VERTB, vblankHandler, 0);
	keyCreate();
	updateGameState(START_GAME_TEXT, ON);
	updateGameState(CLEARED_START_TEXT, OFF);
	updateGameState(PLAYING_STATE, OFF);
	updateGameState(GAME_OVER_TEXT, OFF);

	addPowerPillsToMap(powerPill);
	addPelletsToMap(pellet);

	while (!MouseLeft())
	{
		// 1. Process Input & Logic
		if (!processInputs())
			break;

		// 2. Update pellets on the map based on Pacman's position
		if (getGameState(PLAYING_STATE) == ON)
		{
			updatePellets(pacman, pelletsOnMap, tBackground, tScreenBuffers[0], tScreenBuffers[1]);
		}

		// 2. Update ghost paths and positions
		if (getGameState(PLAYING_STATE) == ON)
		{
			ghostUpdates();
		}

		// 3. Handle game start text and transition to gameplay
		gameStartUpdates();

		// 4. Update background for all objects based on their last drawn positions (before we draw them in their new positions)
		// ==========================================
		// CLEAR PHASE: Restore backgrounds for ALL objects
		// ==========================================
		backgroundUpdates();

		// 5. Draw ALL objects in their new positions
		// ==========================================
		// DRAW PHASE: Draw ALL objects on the screen
		// ==========================================
		bobUpdates();

		// 6. Wait for VBlank and swap buffers
		// ==========================================
		// SWAP PHASE: Wait for VBlank, then swap buffers
		// ==========================================
		WaitVbl();
		doubleBufferUpdates();

		// 7. Check for collisions between Pacman and the ghosts
		if (pacmanCollide(pacman, redGhost, blueGhost, pinkGhost, orangeGhost))
		{
			KPrintF("Pacman collided with a ghost!\n");
			setGameOverState();
		}

		// 8. Check Game Over state and reset if player clicks to restart
		if (getGameState(GAME_OVER_TEXT) == ON)
		{
			displayGameOverText();
		}

		keyProcess(); // Process pending keystrokes from the CIA interrupt buffer
	}

	KPrintF("Exit Loop!\n");
	keyDestroy();

	// Clean up custom interrupt before waking up the OS
	// Otherwise the hardware will call it after the program has exited!
	KPrintF("Reset Interrupt!\n");
	systemSetInt(INTB_VERTB, 0, 0);
	WaitVbl(); // Wait 1 frame to guarantee the handler is fully finished

	KPrintF("Free Copper List!\n");
	FreeMem(copper1, 1024);

#ifdef MUSIC
	KPrintF("End Music!\n");
	p61End(); // End the music player safely BEFORE waking up the OS!
#endif

	// END
	teardownEnvironment();
}
