#include "sound.h"

void PlaySound(Mix_Chunk *sound)
{
    if (sound)
    {
        Mix_PlayChannel(-1, sound, 0);
    }
}