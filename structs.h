#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// 入力状態を保持する構造体
typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} InputState;

// プレイヤーの状態を保持する構造体
typedef struct
{
    SDL_Rect rect;
    // 今後、ここにテクスチャやHPなどを追加していく
} Player;

// ゲーム全体の状態を管理する構造体
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Joystick *ddrMat;
    bool isRunning;
    InputState input; // 入力状態を追加
    Player player;    // プレイヤーを追加
} GameState;

#endif // STRUCTS_H