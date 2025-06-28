#include "sound.h"

void PlaySound(Mix_Chunk *sound)
{
    if (sound)
    {
        // -1: 空いているチャンネルで再生, sound: 再生する音, 0: ループしない
        Mix_PlayChannel(-1, sound, 0);
    }
}