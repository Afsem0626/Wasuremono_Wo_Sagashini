#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_VEGGIES 3 // 野菜の数

typedef struct
{
    bool isActive;
    SDL_Rect rect;
    SDL_Texture *texture;
} GameObject;

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
    SDL_Texture *texture;
    int hp;
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
    GameObject veggies[MAX_VEGGIES];
    int veggiesCollected;
    // アセット
    TTF_Font *font;
} GameState;

#endif // STRUCTS_H