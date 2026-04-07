#include "copper_routines.h"
#include "support/gcc8_c_support.h"
#include <hardware/custom.h>

short copSetPlanes(UBYTE bplPtrStart, USHORT **copListEnd_out, const UBYTE **planes, int numPlanes)
{
    if (!copListEnd_out || !*copListEnd_out || !planes)
        return -1;

    USHORT *copListEnd = *copListEnd_out;
    for (int i = 0; i < numPlanes; i++)
    {
        ULONG addr = (ULONG)planes[i];
        *copListEnd++ = (USHORT)(offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR));
        *copListEnd++ = (UWORD)(addr >> 16);
        *copListEnd++ = (USHORT)(offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR) + 2);
        *copListEnd++ = (UWORD)addr;
    }
    *copListEnd_out = copListEnd;
    return 0;
}

short copWaitXY(USHORT **copListEnd_out, USHORT x, USHORT i)
{
    if (!copListEnd_out || !*copListEnd_out)
        return -1;

    USHORT *copListEnd = *copListEnd_out;
    *copListEnd++ = (USHORT)((i << 8) | (x << 1) | 1); // bit 1 means wait. waits for vertical position x<<8, first raster stop position outside the left
    *copListEnd++ = 0xfffe;
    *copListEnd_out = copListEnd;
    return 0;
}

short copWaitY(USHORT **copListEnd_out, USHORT i)
{
    if (!copListEnd_out || !*copListEnd_out)
        return -1;

    USHORT *copListEnd = *copListEnd_out;
    *copListEnd++ = (USHORT)((i << 8) | 4 | 1); // bit 1 means wait. waits for vertical position x<<8, first raster stop position outside the left
    *copListEnd++ = 0xfffe;
    *copListEnd_out = copListEnd;
    return 0;
}

short copSetColor(USHORT **copListCurrent_out, USHORT index, USHORT color)
{
    if (!copListCurrent_out || !*copListCurrent_out)
        return -1;

    USHORT *copListCurrent = *copListCurrent_out;
    *copListCurrent++ = (USHORT)(offsetof(struct Custom, color) + sizeof(UWORD) * index);
    *copListCurrent++ = color;
    *copListCurrent_out = copListCurrent;
    return 0;
}