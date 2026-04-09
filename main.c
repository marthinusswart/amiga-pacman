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
#include "routines/render_routines.h"
#include "player/pacman.h"
#include "ghost/ghost.h"
#include "routines/state_routines.h"
#include "state/state_ext.h"

// config
#define MUSIC_OFF
#define DEBUG_ON

// Debug print macro
#ifdef DEBUG_ON
#define DEBUG_PRINT(fmt, ...) KPrintF(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

// forward declarations
static void resetGameState(void);

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

	DEBUG_PRINT("Hello debugger from Amiga!\n");

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
	DEBUG_PRINT("Destroying System!\n");
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
		DEBUG_PRINT("free gfx library!\n");
		CloseLibrary((struct Library *)GfxBase);
	}

	if (DOSBase)
	{
		DEBUG_PRINT("free dos library!\n");
		CloseLibrary((struct Library *)DOSBase);
	}
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
			DEBUG_PRINT("Resetting game state!\n");
			resetGameState();
		}
		else
		{
			DEBUG_PRINT("Starting game state!\n");
			updateGameState(START_GAME_TEXT, OFF);
			updateGameState(PLAYING_STATE, ON);
		}
	}

	prevSpaceState = currSpaceState;

	return 1; // Signal to continue
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

	addPelletsToMap(pellet, pelletsOnMap, tBackground, tPacmanTiles, tScreenBuffers,
					(const UBYTE *)pacman_tiles_mask, mapping_stage_0001);

	updateGameState(START_GAME_TEXT, ON);
	updateGameState(CLEARED_START_TEXT, OFF);
	updateGameState(PLAYING_STATE, OFF);
	updateGameState(GAME_OVER_TEXT, OFF);
	updateGameState(CLEAR_GAME_OVER_TEXT, ON);
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
		DEBUG_PRINT("Pellet picked up!\n");

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
		DEBUG_PRINT("p61Init failed!\n");
#endif
	warpmode(0);
	WaitVbl();

	if (setupBuffers(tScreenBuffers, &tPacmanTiles, &tBackground, pacman_tiles, pacman_stage_01) != 0)
	{
		DEBUG_PRINT("Failed to setup buffers\n");
		return 0;
	}

	USHORT *copper;
	if (setupCopper(&copper, tScreenBuffers[0], custom, (const USHORT *)colors, &bplPtrsInCopper) != 0)
	{
		DEBUG_PRINT("Failed to setup copper\n");
		return 0;
	}
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

	addPowerPillsToMap(powerPill, tBackground, tPacmanTiles, tScreenBuffers,
					   (const UBYTE *)pacman_tiles_mask, mapping_stage_0001);
	addPelletsToMap(pellet, pelletsOnMap, tBackground, tPacmanTiles, tScreenBuffers,
					(const UBYTE *)pacman_tiles_mask, mapping_stage_0001);

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
			ghostUpdates(pacman, redGhost, blueGhost, pinkGhost, orangeGhost);
		}

		// 3. Handle game start text and transition to gameplay
		gameStartUpdates();

		// 4. Update background for all objects based on their last drawn positions (before we draw them in their new positions)
		// ==========================================
		// CLEAR PHASE: Restore backgrounds for ALL objects
		// ==========================================
		backgroundUpdates(tBackground, lastPosition, backBufferIdx, tScreenBuffers[backBufferIdx],
						  orangeGhost, blueGhost, redGhost, pinkGhost, pacman);

		// 5. Draw ALL objects in their new positions
		// ==========================================
		// DRAW PHASE: Draw ALL objects on the screen
		// ==========================================
		bobUpdates(blueGhost, redGhost, pinkGhost, orangeGhost, pacman,
				   lastPosition, backBufferIdx, tPacmanTiles, tScreenBuffers[backBufferIdx],
				   (const UBYTE *)pacman_tiles_mask);

		// 6. Wait for VBlank and swap buffers
		// ==========================================
		// SWAP PHASE: Wait for VBlank, then swap buffers
		// ==========================================
		WaitVbl();
		doubleBufferUpdates(tScreenBuffers, &frontBufferIdx, &backBufferIdx, bplPtrsInCopper);

		// 7. Check for collisions between Pacman and the ghosts
		if (pacman->isPacmanColliding(pacman, redGhost, blueGhost, pinkGhost, orangeGhost))
		{
			DEBUG_PRINT("Pacman collided with a ghost!\n");
			setGameOverState();
		}

		// 8. Check Game Over state and reset if player clicks to restart
		if (getGameState(GAME_OVER_TEXT) == ON)
		{
			displayGameOverText();
		}

		keyProcess(); // Process pending keystrokes from the CIA interrupt buffer
	}

	DEBUG_PRINT("Exit Loop!\n");
	keyDestroy();

	// Clean up custom interrupt before waking up the OS
	// Otherwise the hardware will call it after the program has exited!
	DEBUG_PRINT("Reset Interrupt!\n");
	systemSetInt(INTB_VERTB, 0, 0);
	WaitVbl(); // Wait 1 frame to guarantee the handler is fully finished

	DEBUG_PRINT("Free Copper List!\n");
	FreeMem(copper, 1024);

#ifdef MUSIC
	DEBUG_PRINT("End Music!\n");
	p61End(); // End the music player safely BEFORE waking up the OS!
#endif

	// END
	teardownEnvironment();
}
