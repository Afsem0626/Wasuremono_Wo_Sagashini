#include "sound.h"

void PlaySoundOnChannel(Mix_Chunk *sound, int channel)
{
    if (sound)
    {
        // Mix_PlayChannelの第1引数に、予約したチャンネル番号を指定する
        Mix_PlayChannel(channel, sound, 0);
    }
}