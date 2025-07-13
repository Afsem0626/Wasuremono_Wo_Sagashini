#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_VEGGIES 3
#define MAX_ENEMIES 10
#define MAX_ARROWS 18
#define VISIBLE_ARROWS 4
#define ARROW_ANIMATION_DURATION 0.3f // 矢印アニメーションの時間（0.3秒）

// ゲームシーン
typedef enum
{
    SCENE_TITLE,
    SCENE_DIFFICULTY,
    SCENE_MAIN_STAGE,
    SCENE_STAGE_CLEAR, // カットイン用
    SCENE_GAME_OVER,
    SCENE_NOVEL,
    SCENE_ENDING,
} GameScene;

// 難易度用
typedef enum
{
    DIFF_DAY,
    DIFF_EVENING,
    DIFF_NIGHT,
    DIFF_IKUU,
    DIFFICULTY_COUNT,
} Difficulty;

typedef struct
{
    SDL_Texture *characterTexture; // 立ち絵のテクスチャ
    SDL_Texture *windowTexture;    // メッセージウィンドウのテクスチャ
    char **lines;                  // セリフの文字列配列
    int lineCount;                 // セリフの総行数
    int currentLine;               // 現在表示している行番号
} NovelState;

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
    int vx, vy;
    DoorState doorState; // 扉の状態
} GameObject;

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *texture;
    int hp;
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
    Difficulty difficulty;   // 難易度
    int difficultySelection; // カーソル用の選択肢
    MinigameType currentMinigame;
    int veggiesRequired;
    int veggiesCollected;
    float stageTimer;
    float transitionTimer; // ★★★ カットイン表示用タイマーを追加 ★★★

    int minigamesRequired; // クリアに必要なミニゲーム数
    int minigamesCleared;  // クリアしたミニゲーム数

    // ノベル用変数
    NovelState novel;

    // オブジェクト
    InputState input;
    Player player;
    GameObject veggies[MAX_VEGGIES];
    GameObject enemies[MAX_ENEMIES];
    GameObject door;
    int arrowSequence[MAX_ARROWS]; // 配列のサイズは最大値で確保
    int arrowCount;                // ★★★ 追加：このステージで使う矢印の数 ★★★
    int arrowPlayerProgress;

    // アニメーション用変数を追加
    bool isArrowAnimating;     // アニメーション中かどうかのフラグ
    float arrowAnimationTimer; // アニメーションの進行時間タイマー
    int clearedArrowIndex;     // どの矢印が消えるアニメーション中か

    // アセット
    TTF_Font *font;
    TTF_Font *largeFont; // ★★★ 追加 ★★★
    Mix_Chunk *damageSound;
    SDL_Texture *titleTexture;
    SDL_Texture *arrowTextures[MAX_ARROWS];
    SDL_Texture *doorLockedTexture;
    SDL_Texture *doorUnlockedTexture;

} GameState;

#endif // STRUCTS_H