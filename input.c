#include "input.h"

// 1フレーム前のキーボード状態を記憶するための静的変数
static Uint8 prev_keyboard_state[SDL_NUM_SCANCODES] = {0};

// 引数を (GameState* gs, InputState* input) に統一
void HandleInput(GameState *gs, InputState *input)
{
    // --- 1. SDLイベントキューを処理 ---
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            gs->isRunning = false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            gs->isRunning = false;
        }
    }

    // --- 2. 現在のキーボード状態を取得 ---
    const Uint8 *current_keyboard_state = SDL_GetKeyboardState(NULL);

    // --- 3. 「押しっぱなし」状態を更新 ---
    input->up_held = current_keyboard_state[SDL_SCANCODE_UP];
    input->down_held = current_keyboard_state[SDL_SCANCODE_DOWN];
    input->left_held = current_keyboard_state[SDL_SCANCODE_LEFT];
    input->right_held = current_keyboard_state[SDL_SCANCODE_RIGHT];
    input->a_held = current_keyboard_state[SDL_SCANCODE_Z];
    input->b_held = current_keyboard_state[SDL_SCANCODE_X];

    // --- 4. 「押された瞬間」を判定 ---
    input->up_pressed = input->up_held && !prev_keyboard_state[SDL_SCANCODE_UP];
    input->down_pressed = input->down_held && !prev_keyboard_state[SDL_SCANCODE_DOWN];
    input->left_pressed = input->left_held && !prev_keyboard_state[SDL_SCANCODE_LEFT];
    input->right_pressed = input->right_held && !prev_keyboard_state[SDL_SCANCODE_RIGHT];
    input->a_pressed = input->a_held && !prev_keyboard_state[SDL_SCANCODE_Z];
    input->b_pressed = input->b_held && !prev_keyboard_state[SDL_SCANCODE_X];

    // --- 5. 現在の状態を「1フレーム前の状態」として保存 ---
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        prev_keyboard_state[i] = current_keyboard_state[i];
    }
}