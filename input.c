#include "input.h"
#include <stdio.h>

void HandleInput(GameState *gs)
{
    SDL_Event event;

    // イベントキューからイベントを一つずつ取り出す
    while (SDL_PollEvent(&event))
    {
        // イベントの種類を判別
        switch (event.type)
        {
        // ウィンドウの×ボタンが押された場合
        case SDL_QUIT:
            gs->isRunning = false;
            break;

        // キーボードのキーが押された場合
        case SDL_KEYDOWN:
            // Escapeキーならゲームを終了
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                gs->isRunning = false;
            }
            break;

        // ジョイスティックのボタンが押された場合 (DDRマットのパネル)
        case SDL_JOYBUTTONDOWN:
            printf("DDR Mat Panel Pressed: Button %d\n", event.jbutton.button);
            break;
        }
    }
}