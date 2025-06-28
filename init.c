#include "init.h"
#include <stdio.h>

// ... LoadTexture関数は変更なし ...

bool InitGame(GameState *gs)
{
    // SDL_mixerも初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ||
        IMG_Init(IMG_INIT_PNG) == 0 ||
        TTF_Init() == -1 ||
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "ライブラリの初期化に失敗しました: %s\n", SDL_GetError());
        return false;
    }
    // ... ウィンドウ、レンダラー、ジョイスティックの作成は同じ ...

    // ゲーム状態の初期化
    gs->isRunning = true;
    gs->currentScene = SCENE_MAIN_STAGE;
    gs->player.hp = 5; // HPの初期値を設定
    gs->veggiesCollected = 0;

    // 敵の初期化
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true;
        gs->enemies[i].rect = (SDL_Rect){1800, 200 + i * 300, 120, 120}; // 初期位置
        gs->enemies[i].vx = -5;                                          // 左に動く速度
        gs->enemies[i].vy = 0;
    }

    // ... プレイヤーと野菜の初期化は同じ ...
    return true;
}

void LoadAssets(GameState *gs)
{
    // ... フォントと野菜テクスチャの読み込みは同じ ...
    gs->player.texture = LoadTexture("player.png", gs->renderer);
    // 敵テクスチャの読み込みを追加
    gs->enemies[0].texture = LoadTexture("enemies/enemy.png", gs->renderer);
    gs->enemies[1].texture = gs->enemies[0].texture; // 同じ画像を使う

    // 効果音の読み込みを追加
    gs->damageSound = Mix_LoadWAV("sound/damage.wav");
    if (!gs->damageSound)
    {
        fprintf(stderr, "効果音をロードできませんでした: %s\n", Mix_GetError());
    }
}

void Cleanup(GameState *gs)
{
    // ... テクスチャとフォントの解放に加えて ...
    Mix_FreeChunk(gs->damageSound); // 効果音の解放
    // ... SDLサブシステムの終了 ...
    Mix_CloseAudio();
    Mix_Quit();
}