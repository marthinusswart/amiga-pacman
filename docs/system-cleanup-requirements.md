# System Cleanup Requirements for amiga-pacman

This document identifies all system calls and resource allocations in [main.c](../main.c) that require cleanup when exiting the game.

## Overview

The game acquires various system resources during initialization and gameplay. Proper cleanup is critical on Amiga to restore the OS to a stable state and prevent crashes or system lockups.

---

## Library Resources

### dos.library
- **Location**: [main.c:132](../main.c#L132)
- **Call**: `OpenLibrary("dos.library", 0)`
- **Storage**: `DOSBase`
- **Cleanup**: [main.c:197-201](../main.c#L197-L201) - `CloseLibrary((struct Library *)DOSBase)`
- **Status**: ✅ Properly cleaned up in `teardownEnvironment()`

### graphics.library
- **Location**: [main.c:133](../main.c#L133)
- **Call**: `OpenLibrary("graphics.library", 0)`
- **Storage**: `GfxBase`
- **Cleanup**: [main.c:191-195](../main.c#L191-L195) - `CloseLibrary((struct Library *)GfxBase)`
- **Status**: ✅ Properly cleaned up in `teardownEnvironment()`

---

## Memory Allocations

### Copper List (copper1)
- **Location**: [main.c:206](../main.c#L206)
- **Call**: `AllocMem(1024, MEMF_CHIP)`
- **Storage**: `copper1` (returned from `setupCopper()`)
- **Size**: 1024 bytes
- **Cleanup**: [main.c:781](../main.c#L781) - `FreeMem(copper1, 1024)`
- **Status**: ✅ Properly freed in `main()` exit sequence

### Screen Buffers (Double Buffering)
- **Location**: [main.c:210-211](../main.c#L210-L211)
- **Call**: `bitmapCreate(320, 256, 5, BMF_CLEAR | BMF_DISPLAYABLE)` (called twice)
- **Storage**: `tScreenBuffers[0]` and `tScreenBuffers[1]`
- **Cleanup**: [main.c:162-165](../main.c#L162-L165) - `bitmapDestroy()` for both buffers
- **Status**: ✅ Properly cleaned up in `teardownEnvironment()`

### Pacman Tiles Bitmap Structure
- **Location**: [main.c:214](../main.c#L214)
- **Call**: `AllocMem(sizeof(tBitMap), MEMF_PUBLIC | MEMF_CLEAR)`
- **Storage**: `tPacmanTiles`
- **Size**: `sizeof(tBitMap)`
- **Cleanup**: [main.c:166-167](../main.c#L166-L167) - `FreeMem(tPacmanTiles, sizeof(tBitMap))`
- **Status**: ✅ Properly freed in `teardownEnvironment()`
- **Note**: Only the structure is allocated; the plane data points to INCBIN data

### Background Bitmap Structure
- **Location**: [main.c:222](../main.c#L222)
- **Call**: `AllocMem(sizeof(tBitMap), MEMF_PUBLIC | MEMF_CLEAR)`
- **Storage**: `tBackground`
- **Size**: `sizeof(tBitMap)`
- **Cleanup**: [main.c:168-169](../main.c#L168-L169) - `FreeMem(tBackground, sizeof(tBitMap))`
- **Status**: ✅ Properly freed in `teardownEnvironment()`
- **Note**: Only the structure is allocated; the plane data points to INCBIN data

---

## System State Modifications

### ACE System Takeover
- **Location**: [main.c:153](../main.c#L153)
- **Call**: `systemCreate()`
- **Purpose**: Seizes control of the Amiga system (disables OS multitasking)
- **Cleanup**: [main.c:171](../main.c#L171) - `systemDestroy()`
- **Status**: ✅ Properly cleaned up in `teardownEnvironment()`

### Hardware State Backup
The following hardware registers are backed up before system takeover:

#### ADKCON Register
- **Location**: [main.c:149](../main.c#L149)
- **Call**: `custom->adkconr` (read)
- **Storage**: `SystemADKCON`
- **Cleanup**: [main.c:188](../main.c#L188) - `custom->adkcon = SystemADKCON | 0x8000`
- **Status**: ✅ Properly restored in `teardownEnvironment()`
- **Note**: Critical for disk drives and audio hardware

#### INTENA Register (Interrupts)
- **Location**: [main.c:150](../main.c#L150)
- **Call**: `custom->intenar` (read)
- **Storage**: `SystemInts`
- **Cleanup**: [main.c:186](../main.c#L186) - `custom->intena = SystemInts | 0x8000`
- **Status**: ✅ Properly restored in `teardownEnvironment()`

#### DMACON Register (DMA)
- **Location**: [main.c:151](../main.c#L151)
- **Call**: `custom->dmaconr` (read)
- **Storage**: `SystemDMA`
- **Cleanup**: [main.c:187](../main.c#L187) - `custom->dmacon = SystemDMA | 0x8000`
- **Status**: ✅ Properly restored in `teardownEnvironment()`

### Copper List Pointers
- **Location**: [main.c:269](../main.c#L269), [main.c:272](../main.c#L272)
- **Calls**: 
  - `custom->cop1lc = (ULONG)copper1` - Set game copper list
  - `custom->copjmp1 = 0x7fff` - Start copper
- **Cleanup**: [main.c:182-184](../main.c#L182-L184)
  - `custom->cop1lc = (ULONG)GfxBase->copinit` - Restore OS copper list
  - `custom->cop2lc = (ULONG)GfxBase->LOFlist`
  - `custom->copjmp1 = 0x7fff`
- **Status**: ✅ Properly restored in `teardownEnvironment()`
- **Note**: Critical to restore before freeing copper1 memory!

### DMA Mask
- **Location**: [main.c:702](../main.c#L702)
- **Call**: `systemSetDmaMask(DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER, 1)`
- **Purpose**: Enables blitter, copper, and raster DMA
- **Cleanup**: Handled by hardware register restoration in `teardownEnvironment()`
- **Status**: ✅ Implicitly cleaned up via DMACON restoration

---

## Interrupt Handlers

### Vertical Blank Interrupt (VBlank)
- **Location**: [main.c:705](../main.c#L705)
- **Call**: `systemSetInt(INTB_VERTB, vblankHandler, 0)`
- **Purpose**: Custom VBlank handler for music and frame counting
- **Cleanup**: [main.c:777](../main.c#L777) - `systemSetInt(INTB_VERTB, 0, 0)`
- **Post-Cleanup**: [main.c:778](../main.c#L778) - `WaitVbl()` to ensure handler completes
- **Status**: ✅ Properly removed in `main()` exit sequence
- **Note**: Must be removed BEFORE calling `teardownEnvironment()` to prevent calling freed code!

---

## Input System

### Keyboard Manager
- **Location**: [main.c:706](../main.c#L706)
- **Call**: `keyCreate()`
- **Purpose**: Initializes ACE keyboard input system
- **Cleanup**: [main.c:772](../main.c#L772) - `keyDestroy()`
- **Status**: ✅ Properly cleaned up in `main()` exit sequence

---

## Music System (Conditional)

### ThePlayer Music Module (ifdef MUSIC)
- **Location**: [main.c:675-676](../main.c#L675-L676)
- **Call**: `p61Init(module)`
- **Purpose**: Initializes P61 music player
- **Cleanup**: [main.c:784-785](../main.c#L784-L785) - `p61End()`
- **Status**: ✅ Properly cleaned up in `main()` exit sequence
- **Note**: Only relevant when MUSIC is defined (currently disabled via MUSIC_OFF)

---

## Warp Mode

### CPU Speed Control
- **Location**: [main.c:156](../main.c#L156), [main.c:678](../main.c#L678)
- **Calls**: 
  - `warpmode(1)` - Enable warp mode during setup
  - `warpmode(0)` - Disable warp mode before game loop
- **Cleanup**: No explicit cleanup needed; already disabled at [main.c:678](../main.c#L678)
- **Status**: ✅ Properly managed (disabled before game starts)

---

## Sprite/Game Objects

### Game Entity Setup
- **Location**: [main.c:657-668](../main.c#L657-L668) - `setupSprites()`
- **Calls**:
  - `setupPacman(&pacman)`
  - `setupBlueGhost(&blueGhost)`
  - `setupRedGhost(&redGhost)`
  - `setupPinkGhost(&pinkGhost)`
  - `setupOrangeGhost(&orangeGhost)`
  - `setupStartText(&startText)`
  - `setupGameOverText(&gameOverText)`
  - `setupPowerPill(&powerPill)`
  - `setupPellets(&pellet)`
- **Storage**: Global pointers (lines 41-49)
- **Cleanup**: ⚠️ **NOT FOUND IN CURRENT CODE**
- **Status**: ⚠️ **POTENTIAL MEMORY LEAK** - No explicit cleanup for sprite allocations

---

## Critical Cleanup Sequence

The cleanup must happen in the correct order to prevent crashes:

1. **[main.c:772](../main.c#L772)** - `keyDestroy()` - Stop keyboard input
2. **[main.c:777](../main.c#L777)** - `systemSetInt(INTB_VERTB, 0, 0)` - Remove VBlank handler
3. **[main.c:778](../main.c#L778)** - `WaitVbl()` - Ensure interrupt finishes
4. **[main.c:781](../main.c#L781)** - `FreeMem(copper1, 1024)` - Free copper list memory
5. **[main.c:785](../main.c#L785)** - `p61End()` (if MUSIC) - Stop music
6. **[main.c:789](../main.c#L789)** - `teardownEnvironment()` - Full system restoration:
   - Destroy screen buffers
   - Free bitmap structures
   - Disable interrupts/DMA (Disable() CPU interrupts first!)
   - Restore copper lists to OS values
   - Restore hardware registers (INTENA, DMACON, ADKCON)
   - Enable() CPU interrupts
   - Close libraries
   - `systemDestroy()` - Return control to OS

---

## Issues and Recommendations

### ⚠️ Potential Issues

1. **Sprite Memory Cleanup Missing**
   - The sprite setup functions (`setupPacman`, `setupBlueGhost`, etc.) likely allocate memory
   - No corresponding cleanup/free functions are called
   - **Recommendation**: Add cleanup functions and call them before `teardownEnvironment()`

2. **OS Suspend State**
   - [main.c:154](../main.c#L154) - `systemUnuse()` suspends the OS
   - No explicit `systemUse()` call found
   - **Recommendation**: Verify that `systemDestroy()` implicitly calls `systemUse()`

### ✅ Well-Designed Aspects

1. **Manual Hardware Restoration**
   - The code explicitly restores hardware state rather than relying solely on ACE
   - Uses `Disable()`/`Enable()` to prevent race conditions during restoration
   - Restores copper list pointers BEFORE freeing memory

2. **Interrupt Safety**
   - VBlank handler is removed before teardown
   - Additional VBlank wait ensures handler completes before proceeding

3. **Conditional Compilation**
   - Music system cleanup is properly guarded by `#ifdef MUSIC`

---

## Summary

**Total System Resources Tracked**: 16

**Properly Cleaned Up**: 15 ✅

**Missing Cleanup**: 1 ⚠️ (Sprite/game object allocations)

The cleanup sequence is well-designed with proper ordering and safety measures. The main concern is potential memory leaks from sprite allocations that may not be freed.
