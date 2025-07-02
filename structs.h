#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_VEGGIES 3
#define MAX_ENEMIES 2 // 敵の数を定義

// ゲームの場面（シーン）を定義
typedef enum
{
    SCENE_TITLE, // タイトル画面
    SCENE_MAIN_STAGE,
    SCENE_GAME_OVER
} GameScene;

typedef enum
{
    MINIGAME_VEGGIE,
    MINIGAME_ARROWS
} MinigameType;

typedef enum
{
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    MAX_ARROWS
} ArrowDir;

// (InputStateはステップ3から変更なし)
typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} InputState;

typedef struct
{
    bool isActive;
    SDL_Rect rect;
    SDL_Texture *texture;
    int vx, vy; // 速度
} GameObject;

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *texture;
    int hp; // HPを追加
} Player;

typedef struct
{
    // SDL関連
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Joystick *ddrMat;

    // ゲーム状態
    bool isRunning;
    GameScene currentScene;

    InputState input;
    MinigameType currentMinigame;
    //  オブジェクト
    Player player;
    GameObject veggies[MAX_VEGGIES];
    GameObject enemies[MAX_ENEMIES];
    int veggiesCollected;

    // アセット
    TTF_Font *font;
    Mix_Chunk *damageSound; // 効果音を追加
    SDL_Texture *arrowTextures[MAX_ARROWS];
    // ミニゲーム2の状態
    int arrowSequence[MAX_ARROWS];

} GameState;

#endif // STRUCTS_H