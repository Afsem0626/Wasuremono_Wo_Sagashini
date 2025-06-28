#include "input.h"
#include <stdio.h>

// input.hで宣言した通り、引数を (InputState* input, bool* isRunning) に修正
void HandleInput(InputState *input, bool *isRunning)
{
    SDL_Event event;

    // 前フレームの「押された瞬間」フラグをリセット
    // (これはより高度な実装ですが、今はなくても動きます)
    // input->wasUpPressed = false; ...など

    // SDLイベントキューを処理
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // ウィンドウの×ボタンが押された場合
        case SDL_QUIT:
            *isRunning = false; // ポインタ経由でフラグを操作
            break;

        // キーボードのキーが押された場合
        case SDL_KEYDOWN:
            // Escapeキーならゲームを終了
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                *isRunning = false;
            }
            break;
        }
    }

    // キーボードの「押しっぱなし」状態を取得し、InputStateを更新
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    input->up = keyboardState[SDL_SCANCODE_UP];
    input->down = keyboardState[SDL_SCANCODE_DOWN];
    input->left = keyboardState[SDL_SCANCODE_LEFT];
    input->right = keyboardState[SDL_SCANCODE_RIGHT];

    // 今後、ここにDDRマットの押しっぱなし状態の処理も追加できます
}