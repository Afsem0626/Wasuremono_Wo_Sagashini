#include "sound.h"

void PlaySoundOnChannel(Mix_Chunk *sound, int channel)
{
    if (sound)
    {
        Mix_PlayChannel(channel, sound, 0);
    }
}