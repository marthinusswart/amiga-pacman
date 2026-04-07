#ifndef COPPER_ROUTINES_H
#define COPPER_ROUTINES_H

#include <exec/types.h>

short copSetPlanes(UBYTE bplPtrStart, USHORT **copListEnd_out, const UBYTE **planes, int numPlanes);
short copWaitXY(USHORT **copListEnd_out, USHORT x, USHORT i);
short copWaitY(USHORT **copListEnd_out, USHORT i);
short copSetColor(USHORT **copListCurrent_out, USHORT index, USHORT color);

#endif // COPPER_ROUTINES_H