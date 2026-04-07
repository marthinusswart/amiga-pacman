#include "system_routines.h"
#include "screen_routines.h"
#include "queue_routines.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <hardware/custom.h>

extern volatile struct Custom *custom;
extern struct GfxBase *GfxBase;

extern APTR SystemIrq;
extern volatile APTR VBR;

// Global queue for storing system state
static Queue *systemStateQueue = NULL;

// Keys for the system state queue
static const char *KEY_ADKCON = "ADKCON";
static const char *KEY_INTENA = "INTENA";
static const char *KEY_DMACON = "DMACON";
static const char *KEY_ACTIVIEW = "ACTIVIEW";

void TakeSystem(void)
{
    Forbid();

    // Create the queue if it doesn't exist
    if (!systemStateQueue)
    {
        systemStateQueue = queueCreate();
    }

    // Save current interrupts and DMA settings so we can restore them upon exit (using local vars)
    UWORD systemADKCON = custom->adkconr;
    UWORD systemInts = custom->intenar;
    UWORD systemDMA = custom->dmaconr;
    struct View *actiView = GfxBase->ActiView; // store current view

    // Push the values to the queue as they are retrieved
    queuePush(systemStateQueue, (void *)KEY_ADKCON, (void *)(ULONG)systemADKCON);
    queuePush(systemStateQueue, (void *)KEY_INTENA, (void *)(ULONG)systemInts);
    queuePush(systemStateQueue, (void *)KEY_DMACON, (void *)(ULONG)systemDMA);
    queuePush(systemStateQueue, (void *)KEY_ACTIVIEW, (void *)actiView);

    LoadView(0);
    WaitTOF();
    WaitTOF();

    WaitVbl();
    WaitVbl();

    OwnBlitter();
    WaitBlt();
    Disable();

    custom->intena = 0x7fff; // disable all interrupts
    custom->intreq = 0x7fff; // Clear any interrupts that were pending
    custom->dmacon = 0x7fff; // Clear all DMA channels

    // set all colors black
    for (int a = 0; a < 32; a++)
        custom->color[a] = 0;

    WaitVbl();
    WaitVbl();

    VBR = GetVBR();
    SystemIrq = GetInterruptHandler(); // store interrupt register
}

void FreeSystem(void)
{
    WaitVbl();
    WaitBlt();
    custom->intena = 0x7fff; // disable all interrupts
    custom->intreq = 0x7fff; // Clear any interrupts that were pending
    custom->dmacon = 0x7fff; // Clear all DMA channels

    // restore interrupts
    SetInterruptHandler(SystemIrq);

    /*Restore system copper list(s). */
    custom->cop1lc = (ULONG)GfxBase->copinit;
    custom->cop2lc = (ULONG)GfxBase->LOFlist;
    custom->copjmp1 = 0x7fff; // start coppper

    // Local variables to hold restored values from queue
    UWORD systemADKCON = 0;
    UWORD systemInts = 0;
    UWORD systemDMA = 0;
    struct View *actiView = NULL;

    // Pop values from the queue in FIFO order (same order they were pushed)
    // We pushed: ADKCON, INTENA, DMACON, ACTIVIEW
    if (systemStateQueue && !queueIsEmpty(systemStateQueue))
    {
        KeyValuePair *pair;

        // Pop ADKCON
        pair = queuePop(systemStateQueue);
        if (pair && pair->key == (void *)KEY_ADKCON)
        {
            systemADKCON = (UWORD)(ULONG)pair->value;
        }

        // Pop INTENA
        pair = queuePop(systemStateQueue);
        if (pair && pair->key == (void *)KEY_INTENA)
        {
            systemInts = (UWORD)(ULONG)pair->value;
        }

        // Pop DMACON
        pair = queuePop(systemStateQueue);
        if (pair && pair->key == (void *)KEY_DMACON)
        {
            systemDMA = (UWORD)(ULONG)pair->value;
        }

        // Pop ACTIVIEW
        pair = queuePop(systemStateQueue);
        if (pair && pair->key == (void *)KEY_ACTIVIEW)
        {
            actiView = (struct View *)pair->value;
        }
    }

    /*Restore all interrupts and DMA settings. */
    custom->intena = systemInts | 0x8000;
    custom->dmacon = systemDMA | 0x8000;
    custom->adkcon = systemADKCON | 0x8000;

    WaitBlt();
    DisownBlitter();
    Enable();

    LoadView(actiView);
    WaitTOF();
    WaitTOF();

    Permit();

    // Clean up the queue if it exists
    if (systemStateQueue)
    {
        queueDestroy(systemStateQueue);
        systemStateQueue = NULL;
    }
}