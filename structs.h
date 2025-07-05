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
    // 「押しっぱなし」状態を保持する変数 (移動などで使用)
    bool up_held;
    bool down_held;
    bool left_held;
    bool right_held;
    bool a_held;
    bool b_held;

    // 「押された瞬間」を保持する変数 (決定、会話送り、リズムゲームで使用)
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool a_pressed;
    bool b_pressed;
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
    MinigameType currentMinigame;

    // ★★★ ここから追加・修正 ★★★
    int veggiesRequired; // ステージクリアに必要な野菜の数
    int veggiesCollected;
    float stageTimer; // ステージの残り時間（秒）
    // ★★★ ここまで追加・修正 ★★★

    // オブジェクト
    Player player;
    GameObject veggies[MAX_VEGGIES];
    GameObject enemies[MAX_ENEMIES];
    int arrowSequence[MAX_ARROWS];
    int arrowPlayerProgress;

    // アセット
    TTF_Font *font;
    Mix_Chunk *damageSound;
    SDL_Texture *arrowTextures[MAX_ARROWS];

} GameState;

#endif // STRUCTS_H