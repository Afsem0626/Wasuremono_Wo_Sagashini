#include "input.h"

// 1フレーム前のキーボード状態を記憶する
static Uint8 prev_keyboard_state[SDL_NUM_SCANCODES] = {0};
// 1フレーム前のDDRマット状態を記憶する
static bool prev_joy_buttons[16] = {0}; // ジョイスティックのボタンは16個までと仮定

// DDRマットのボタン番号を仮に定義（後でjstestで調べた値に置き換える）
enum
{
    DDR_LEFT_BUTTON = 2,
    DDR_DOWN_BUTTON = 1,
    DDR_UP_BUTTON = 0,
    DDR_RIGHT_BUTTON = 3,
    DDR_B_BUTTON = 5,
    DDR_A_BUTTON = 4,
};

void HandleInput(InputState *input, bool *isRunning)
{
    // --- 1. SDLイベントキューを処理 ---
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            *isRunning = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            *isRunning = false;
    }

    // --- 2. 現在の入力状態を取得 ---
    const Uint8 *current_keyboard_state = SDL_GetKeyboardState(NULL);
    SDL_Joystick *joy = SDL_JoystickOpen(0); // とりあえず0番のジョイスティック

    // --- 3. 状態をInputState構造体に格納 ---
    // is_held（押しっぱなし）の判定
    input->up_held = current_keyboard_state[SDL_SCANCODE_UP] || (joy ? SDL_JoystickGetButton(joy, DDR_UP_BUTTON) : 0);
    input->down_held = current_keyboard_state[SDL_SCANCODE_DOWN] || (joy ? SDL_JoystickGetButton(joy, DDR_DOWN_BUTTON) : 0);
    input->left_held = current_keyboard_state[SDL_SCANCODE_LEFT] || (joy ? SDL_JoystickGetButton(joy, DDR_LEFT_BUTTON) : 0);
    input->right_held = current_keyboard_state[SDL_SCANCODE_RIGHT] || (joy ? SDL_JoystickGetButton(joy, DDR_RIGHT_BUTTON) : 0);
    input->a_held = current_keyboard_state[SDL_SCANCODE_Z] || (joy ? SDL_JoystickGetButton(joy, DDR_A_BUTTON) : 0); // キーボードZをAボタン代わりにする
    input->b_held = current_keyboard_state[SDL_SCANCODE_X] || (joy ? SDL_JoystickGetButton(joy, DDR_B_BUTTON) : 0); // キーボードXをBボタン代わりにする

    // was_pressed（押された瞬間）の判定
    input->up_pressed = input->up_held && !prev_joy_buttons[DDR_UP_BUTTON];
    input->down_pressed = input->down_held && !prev_joy_buttons[DDR_DOWN_BUTTON];
    input->left_pressed = input->left_held && !prev_joy_buttons[DDR_LEFT_BUTTON];
    input->right_pressed = input->right_held && !prev_joy_buttons[DDR_RIGHT_BUTTON];
    input->a_pressed = input->a_held && !prev_joy_buttons[DDR_A_BUTTON];
    input->b_pressed = input->b_held && !prev_joy_buttons[DDR_B_BUTTON];

    // --- 4. 現在の状態を「1フレーム前の状態」として保存 ---
    if (joy)
    {
        for (int i = 0; i < 6; i++)
            prev_joy_buttons[i] = SDL_JoystickGetButton(joy, i);
        SDL_JoystickClose(joy);
    }
}

/*void HandleInput(InputState *input, bool *isRunning)
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
}*/