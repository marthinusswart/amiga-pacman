#include "time_routines.h"
#include <ace/managers/timer.h>
#include <ace/managers/system.h>

BOOL isGhostVulnerabilityExpired(ULONG startTime, int durationInSeconds)
{
    // systemGetVerticalBlankFrequency() returns 50 for PAL and 60 for NTSC
    ULONG ticksPerSecond = systemGetVerticalBlankFrequency();
    if (ticksPerSecond == 0)
        ticksPerSecond = 50; // Safe fallback

    ULONG targetTicks = durationInSeconds * ticksPerSecond;

    // timerGetDelta safely calculates elapsed time even if the timer wraps around
    return timerGetDelta(startTime, timerGet()) >= targetTicks;
}