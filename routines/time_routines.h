#ifndef TIME_ROUTINES_H
#define TIME_ROUTINES_H

#include <exec/types.h>
#include "support/gcc8_c_support.h"

BOOL isGhostVulnerabilityExpired(ULONG startTime, int durationInSeconds);

#endif // TIME_ROUTINES_H