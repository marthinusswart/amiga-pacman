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
