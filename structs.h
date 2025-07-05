#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_VEGGIES 3
#define MAX_ENEMIES 2
#define MAX_ARROWS 4

// --- 最初に、他の構造体で使われる型（enum）を定義 ---
typedef enum
{
    SCENE_TITLE,
    SCENE_MAIN_STAGE,
    SCENE_GAME_OVER,
    SCENE_NOVEL,
    SCENE_ENDING, // ★★★ 追加 ★★
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
    ARROW_RIGHT
} ArrowDir;

typedef enum
{
    DOOR_LOCKED,
    DOOR_UNLOCKED
} DoorState;

// --- 次に、基本的な構造体を定義 ---
typedef struct
{
    bool up_held, down_held, left_held, right_held, a_held, b_held;
    bool up_pressed, down_pressed, left_pressed, right_pressed, a_pressed, b_pressed;
} InputState;

typedef struct
{
    bool isActive;        // このオブジェクトが有効か
    SDL_Rect rect;        // 位置とサイズ
    SDL_Texture *texture; // 画像
    int vx, vy;           // 速度
    DoorState doorState;  // ★扉専用の状態
} GameObject;

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *texture;
    int hp;
} Player;

// --- 最後に、上記全てを内包するGameState構造体を定義 ---
typedef struct
{
    // SDL関連
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Joystick *ddrMat;

    // ゲーム状態
    bool isRunning;
    GameScene currentScene;
    MinigameType currentMinigame;
    int veggiesRequired;
    int veggiesCollected;
    float stageTimer;

    int minigamesRequired; // クリアに必要なミニゲーム数
    int minigamesCleared;  // クリアしたミニゲーム数

    // オブジェクト
    InputState input;
    Player player;
    GameObject veggies[MAX_VEGGIES];
    GameObject enemies[MAX_ENEMIES];
    GameObject door; // 扉もGameObjectとして管理
    int arrowSequence[MAX_ARROWS];
    int arrowPlayerProgress;

    // アセット
    TTF_Font *font;
    Mix_Chunk *damageSound;
    SDL_Texture *arrowTextures[MAX_ARROWS];
    SDL_Texture *doorLockedTexture;
    SDL_Texture *doorUnlockedTexture;

} GameState;

#endif // STRUCTS_H