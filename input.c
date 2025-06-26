#include "input.h"
#include <stdio.h>

void HandleInput(GameState *gs)
{
    SDL_Event event;

    // まず、このフレームでの入力状態をリセット
    gs->input.up = false;
    gs->input.down = false;
    gs->input.left = false;
    gs->input.right = false;

    // キーボードの押しっぱなし状態を取得
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_UP])
        gs->input.up = true;
    if (keyboardState[SDL_SCANCODE_DOWN])
        gs->input.down = true;
    if (keyboardState[SDL_SCANCODE_LEFT])
        gs->input.left = true;
    if (keyboardState[SDL_SCANCODE_RIGHT])
        gs->input.right = true;

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
            // 後でパネルの番号を調べて、対応するgs->inputのフラグをtrueにする
            // 例: if (event.jbutton.button == 0) gs->input.up = true;
            break;
        }
    }
}