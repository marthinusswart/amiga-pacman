#include "sprite_ext.h"
#include <proto/exec.h>

short createSprite(Sprite **sprite_out)
{
    if (!sprite_out)
        return -1;

    Sprite *s = (Sprite *)AllocMem(sizeof(Sprite), MEMF_CHIP | MEMF_CLEAR);
    if (!s)
        return -1; // Memory allocation failed

    *sprite_out = s;
    return 0; // Success
}
