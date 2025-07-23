#include "input.h"

// 1フレーム前の入力状態を記憶する静的変数
static InputState prev_input_state = {0};

// DDRマットのボタン番号（jstestで調べた値に後で置き換える）
enum
{
    DDR_UP = 12,
    DDR_DOWN = 14,
    DDR_LEFT = 15,
    DDR_RIGHT = 13,
    DDR_A = 1,
    DDR_B = 2,
};

void HandleInput(GameState *gs, InputState *input)
{
    // --- 1. 前フレームの「押しっぱなし」状態を保存 ---
    prev_input_state.up_held = input->up_held;
    prev_input_state.down_held = input->down_held;
    prev_input_state.left_held = input->left_held;
    prev_input_state.right_held = input->right_held;
    prev_input_state.a_held = input->a_held;
    prev_input_state.b_held = input->b_held;

    // --- 2. SDLイベントキューを処理 (ウィンドウ終了イベントなど) ---
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            gs->isRunning = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            gs->isRunning = false;
    }

    // --- 3. 現在の「押しっぱなし」状態を取得 ---
    const Uint8 *k_state = SDL_GetKeyboardState(NULL);

    // ★★★ or (||) を使って、キーボードとDDRマットのどちらかの入力があればtrueにする ★★★
    input->up_held = k_state[SDL_SCANCODE_UP] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_UP) : 0);
    input->down_held = k_state[SDL_SCANCODE_DOWN] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_DOWN) : 0);
    input->left_held = k_state[SDL_SCANCODE_LEFT] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_LEFT) : 0);
    input->right_held = k_state[SDL_SCANCODE_RIGHT] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_RIGHT) : 0);
    input->a_held = k_state[SDL_SCANCODE_Z] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_A) : 0);
    input->b_held = k_state[SDL_SCANCODE_X] || (gs->ddrMat ? SDL_JoystickGetButton(gs->ddrMat, DDR_B) : 0);

    // --- 4. 「押された瞬間」を判定 ---
    input->up_pressed = input->up_held && !prev_input_state.up_held;
    input->down_pressed = input->down_held && !prev_input_state.down_held;
    input->left_pressed = input->left_held && !prev_input_state.left_held;
    input->right_pressed = input->right_held && !prev_input_state.right_held;
    input->a_pressed = input->a_held && !prev_input_state.a_held;
    input->b_pressed = input->b_held && !prev_input_state.b_held;
}