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
#include "routines/ScreenRoutines.h"
#include "routines/SystemRoutines.h"
#include "routines/MouseRoutines.h"
#include "routines/MusicRoutines.h"
#include "routines/CopperRoutines.h"
#include "routines/CollitionRoutines.h"
#include "player/pacman.h"
#include "ghost/ghost.h"

// config
#define MUSIC

struct ExecBase *SysBase;
volatile struct Custom *custom;
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;
Pacman *pacman;
Ghost *blueGhost;
Ghost *redGhost;

// backup
UWORD SystemInts;
UWORD SystemDMA;
UWORD SystemADKCON;
volatile APTR VBR = 0;
APTR SystemIrq;

struct View *ActiView;

tBitMap *tScreenBuffer = NULL;
tBitMap *tPacmanTiles = NULL;
tBitMap *tBackground = NULL;

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

#ifdef __cplusplus
class TestClass
{
public:
	TestClass(int y)
	{
		static int x = 7;
		i = y + x;
	}
	~TestClass()
	{
		KPrintF("~TestClass()");
	}

	int i;
};

TestClass staticClass(4);
#endif

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

#ifdef __cplusplus
	KPrintF("Hello debugger from Amiga: %ld!\n", staticClass.i);
#else
	KPrintF("Hello debugger from Amiga!\n");
#endif

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
	if (tScreenBuffer)
		bitmapDestroy(tScreenBuffer);
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

	// 1. Create PLANAR screen buffer safely using ACE (BMF_DISPLAYABLE forces CHIP RAM)
	tScreenBuffer = bitmapCreate(320, 256, 5, BMF_CLEAR | BMF_DISPLAYABLE);

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
		CopyMem(tBackground->Planes[p], tScreenBuffer->Planes[p], (320 / 8) * 256);
	}

	copPtr = screenScanDefault(copPtr);

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
		planes[a] = tScreenBuffer->Planes[a];
	}
	copPtr = copSetPlanes(0, copPtr, planes, 5); // INJECT pointers into copper list!

	// set colors
	for (int a = 0; a < 32; a++)
		copPtr = copSetColor(copPtr, a, ((USHORT *)colors)[a]);

	// end copper list
	*copPtr++ = 0xffff;
	*copPtr++ = 0xfffe;

	custom->cop1lc = (ULONG)copper1;
	custom->dmacon = DMAF_BLITTER; // disable blitter dma for copjmp bug
	custom->copjmp1 = 0x7fff;	   // start coppper

	return copper1;
}

static void setupPacman(void)
{
	int bobX = 0;
	int bobY = 0;
	KPrintF("Create Pacman!\n");
	pacman = createPacman(208, 150, 16, 16);
	pacman->setMap(pacman, mapping_stage_0001);
	calculateSpriteLocation(3, 9, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
	pacman->addSprite(pacman, RIGHT, bobX, bobY, 16, 16);
	calculateSpriteLocation(3, 5, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
	pacman->addSprite(pacman, DOWN, bobX, bobY, 16, 16);
	calculateSpriteLocation(3, 7, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
	pacman->addSprite(pacman, LEFT, bobX, bobY, 16, 16);
	calculateSpriteLocation(3, 11, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
	pacman->addSprite(pacman, UP, bobX, bobY, 16, 16);
}

static void setupBlueGhost(void)
{
	int bobX = 0;
	int bobY = 0;
	KPrintF("Create Ghost!\n");
	blueGhost = createGhost(100, 100, 16, 16);
	blueGhost->setMap(blueGhost, mapping_stage_0001);
	calculateSpriteLocation(4, 1, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
	blueGhost->addSprite(blueGhost, RIGHT, bobX, bobY, 16, 16);
	calculateSpriteLocation(6, 1, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
	blueGhost->addSprite(blueGhost, DOWN, bobX, bobY, 16, 16);
	calculateSpriteLocation(5, 1, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
	blueGhost->addSprite(blueGhost, LEFT, bobX, bobY, 16, 16);
	calculateSpriteLocation(7, 1, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
	blueGhost->addSprite(blueGhost, UP, bobX, bobY, 16, 16);
}

static void setupRedGhost(void)
{
	int bobX = 0;
	int bobY = 0;
	KPrintF("Create Ghost!\n");
	redGhost = createGhost(100, 116, 16, 16);
	redGhost->setMap(redGhost, mapping_stage_0001);
	calculateSpriteLocation(4, 7, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created RIGHT sprite at (%ld, %ld)\n", bobX, bobY);
	redGhost->addSprite(redGhost, RIGHT, bobX, bobY, 16, 16);
	calculateSpriteLocation(6, 7, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created DOWN sprite at (%ld, %ld)\n", bobX, bobY);
	redGhost->addSprite(redGhost, DOWN, bobX, bobY, 16, 16);
	calculateSpriteLocation(5, 7, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created LEFT sprite at (%ld, %ld)\n", bobX, bobY);
	redGhost->addSprite(redGhost, LEFT, bobX, bobY, 16, 16);
	calculateSpriteLocation(7, 7, 16, 16, 320, 320, &bobX, &bobY);
	KPrintF("Created UP sprite at (%ld, %ld)\n", bobX, bobY);
	redGhost->addSprite(redGhost, UP, bobX, bobY, 16, 16);
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

	setupPacman();
	setupBlueGhost();
	setupRedGhost();

	systemSetDmaMask(DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER, 1); // Tell ACE to enable DMA

	// DEMO
	systemSetInt(INTB_VERTB, vblankHandler, 0);
	keyCreate();

	while (!MouseLeft())
	{
		WaitVbl();
		if (keyCheck(KEY_ESCAPE))
			break;
		if (keyCheck(KEY_LEFT) || keyCheck(KEY_A))
			pacman->movePacman(pacman, LEFT);
		else if (keyCheck(KEY_RIGHT) || keyCheck(KEY_D))
			pacman->movePacman(pacman, RIGHT);
		else if (keyCheck(KEY_UP) || keyCheck(KEY_W))
			pacman->movePacman(pacman, UP);
		else if (keyCheck(KEY_DOWN) || keyCheck(KEY_S))
			pacman->movePacman(pacman, DOWN);

		// draw pacman
		Sprite *currentSprite = pacman->getSprite(pacman, pacman->direction);
		if (currentSprite)
		{
			// Restore background at previous position instead of clearing to black
			blitCopy(tBackground, pacman->prevX, pacman->prevY,
					 tScreenBuffer, pacman->prevX, pacman->prevY,
					 pacman->width, pacman->height, MINTERM_COOKIE);

			blitCopyMask(
				tPacmanTiles, currentSprite->x, currentSprite->y,
				tScreenBuffer, pacman->x, pacman->y,
				pacman->width, pacman->height,
				(const UBYTE *)pacman_tiles_mask);
		}

		// draw blue ghost
		currentSprite = blueGhost->getSprite(blueGhost, blueGhost->direction);
		if (currentSprite)
		{
			// Restore background at previous position instead of clearing to black
			blitCopy(tBackground, blueGhost->prevX, blueGhost->prevY,
					 tScreenBuffer, blueGhost->prevX, blueGhost->prevY,
					 blueGhost->width, blueGhost->height, MINTERM_COOKIE);

			blitCopyMask(
				tPacmanTiles, currentSprite->x, currentSprite->y,
				tScreenBuffer, blueGhost->x, blueGhost->y,
				blueGhost->width, blueGhost->height,
				(const UBYTE *)pacman_tiles_mask);
		}

		// draw red ghost
		currentSprite = redGhost->getSprite(redGhost, redGhost->direction);
		if (currentSprite)
		{
			// Restore background at previous position instead of clearing to black
			blitCopy(tBackground, redGhost->prevX, redGhost->prevY,
					 tScreenBuffer, redGhost->prevX, redGhost->prevY,
					 redGhost->width, redGhost->height, MINTERM_COOKIE);

			blitCopyMask(
				tPacmanTiles, currentSprite->x, currentSprite->y,
				tScreenBuffer, redGhost->x, redGhost->y,
				redGhost->width, redGhost->height,
				(const UBYTE *)pacman_tiles_mask);
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
