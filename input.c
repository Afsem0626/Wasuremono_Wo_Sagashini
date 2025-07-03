#include "input.h"

// 1フレーム前のキーボード状態を記憶するための静的変数
static Uint8 prev_keyboard_state[SDL_NUM_SCANCODES] = {0};

// DDRマットのボタン番号を仮に定義（後でjstestで調べた値に置き換える）
#define DDR_UP_BUTTON 0
#define DDR_DOWN_BUTTON 1
#define DDR_LEFT_BUTTON 2
#define DDR_RIGHT_BUTTON 3
#define DDR_A_BUTTON 4
#define DDR_B_BUTTON 5

void HandleInput(InputState *input, bool *isRunning)
{
    SDL_Event event;

    // --- 1. まず、このフレームの「押された瞬間」フラグを全てリセット ---
    input->up_pressed = false;
    input->down_pressed = false;
    input->left_pressed = false;
    input->right_pressed = false;
    input->a_pressed = false;
    input->b_pressed = false;

    // --- 2. SDLイベントキューを処理 ---
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            *isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                *isRunning = false;
            }
            break;
        // DDRマットの「押された瞬間」を検知
        case SDL_JOYBUTTONDOWN:
            if (event.jbutton.button == DDR_UP_BUTTON)
                input->up_pressed = true;
            if (event.jbutton.button == DDR_DOWN_BUTTON)
                input->down_pressed = true;
            if (event.jbutton.button == DDR_LEFT_BUTTON)
                input->left_pressed = true;
            if (event.jbutton.button == DDR_RIGHT_BUTTON)
                input->right_pressed = true;
            if (event.jbutton.button == DDR_A_BUTTON)
                input->a_pressed = true;
            if (event.jbutton.button == DDR_B_BUTTON)
                input->b_pressed = true;
            break;
        }
    }

    // --- 3. キーボードの「押しっぱなし」状態を取得 ---
    const Uint8 *current_keyboard_state = SDL_GetKeyboardState(NULL);

    // 各キーについて、「押しっぱなし」と「押された瞬間」を判定
    input->up_held = current_keyboard_state[SDL_SCANCODE_UP];
    if (input->up_held && !prev_keyboard_state[SDL_SCANCODE_UP])
    {
        input->up_pressed = true;
    }

    input->down_held = current_keyboard_state[SDL_SCANCODE_DOWN];
    if (input->down_held && !prev_keyboard_state[SDL_SCANCODE_DOWN])
    {
        input->down_pressed = true;
    }

    input->left_held = current_keyboard_state[SDL_SCANCODE_LEFT];
    if (input->left_held && !prev_keyboard_state[SDL_SCANCODE_LEFT])
    {
        input->left_pressed = true;
    }

    input->right_held = current_keyboard_state[SDL_SCANCODE_RIGHT];
    if (input->right_held && !prev_keyboard_state[SDL_SCANCODE_RIGHT])
    {
        input->right_pressed = true;
    }

    // --- 4. 現在のキーボード状態を次のフレームのために保存 ---
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        prev_keyboard_state[i] = current_keyboard_state[i];
    }
}