#include "init.h"
#include <stdio.h>

// テクスチャ読み込みの補助関数
SDL_Texture *LoadTexture(const char *path, SDL_Renderer *renderer)
{
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (!loadedSurface)
    {
        fprintf(stderr, "画像をロードできませんでした: %s! SDL_image Error: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!newTexture)
    {
        fprintf(stderr, "テクスチャを作成できませんでした: %s! SDL Error: %s\n", path, SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

// アセットを読み込む専用の関数
void LoadAssets(GameState *gs)
{
    // フォント
    gs->font = TTF_OpenFont("ZenOldMincho-Bold.ttf", 28);
    if (!gs->font)
    { /* エラー処理 */
    }

    // プレイヤー画像
    gs->player.texture = LoadTexture("player.png", gs->renderer);

    // 野菜画像
    gs->veggies[0].texture = LoadTexture("vegetables/carrot.png", gs->renderer);
    gs->veggies[1].texture = LoadTexture("vegetables/eggplant.png", gs->renderer);
    gs->veggies[2].texture = LoadTexture("vegetables/tomato.png", gs->renderer);

    // 敵画像
    gs->enemies[0].texture = LoadTexture("enemies/enemy.png", gs->renderer);
    gs->enemies[1].texture = gs->enemies[0].texture; // 2体目は同じ画像を使う

    // 効果音
    gs->damageSound = Mix_LoadWAV("sound/damage.wav");
    if (!gs->damageSound)
    { /* エラー処理 */
    }

    // 矢印テクスチャの読み込み
    gs->arrowTextures[ARROW_UP] = LoadTexture("assets/arrow_up.png", gs->renderer);
    gs->arrowTextures[ARROW_DOWN] = LoadTexture("assets/arrow_down.png", gs->renderer);
    gs->arrowTextures[ARROW_LEFT] = LoadTexture("assets/arrow_left.png", gs->renderer);
    gs->arrowTextures[ARROW_RIGHT] = LoadTexture("assets/arrow_right.png", gs->renderer);
}

bool InitGame(GameState *gs)
{
    // ライブラリの初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ||
        !(IMG_Init(IMG_INIT_PNG)) ||
        TTF_Init() == -1 ||
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "ライブラリの初期化に失敗しました: %s\n", SDL_GetError());
        return false;
    }

    // ウィンドウとレンダラーの作成
    gs->window = SDL_CreateWindow("忘れ物を探シニ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    gs->renderer = SDL_CreateRenderer(gs->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gs->window || !gs->renderer)
        return false;

    // ジョイスティックの準備
    if (SDL_NumJoysticks() > 0)
        gs->ddrMat = SDL_JoystickOpen(0);

    // アセット読み込み
    LoadAssets(gs);

    // --- 各オブジェクトの初期状態を設定 ---
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);

    // プレイヤー
    gs->player.hp = 5;
    gs->player.rect = (SDL_Rect){100, (screen_h - 100) / 2, 100, 100};

    // 野菜
    gs->veggiesCollected = 0;
    gs->veggies[0].isActive = true;
    gs->veggies[0].rect = (SDL_Rect){400, 500, 80, 80};
    gs->veggies[1].isActive = true;
    gs->veggies[1].rect = (SDL_Rect){900, 300, 80, 80};
    gs->veggies[2].isActive = true;
    gs->veggies[2].rect = (SDL_Rect){1400, 600, 80, 80};

    // 敵
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true;
        gs->enemies[i].rect = (SDL_Rect){screen_w + (i * 400), 200 + i * 300, 120, 120};
        gs->enemies[i].vx = -5;
        gs->enemies[i].vy = 0;
    }

    gs->isRunning = true;

    // ★★★ お題となる矢印シーケンスを初期化（テスト用） ★★★
    gs->arrowSequence[0] = ARROW_LEFT;
    gs->arrowSequence[1] = ARROW_RIGHT;
    gs->arrowSequence[2] = ARROW_DOWN;
    gs->arrowSequence[3] = ARROW_UP;
    // 初期シーンをタイトル画面に設定
    gs->currentScene = SCENE_TITLE;
    // gs->currentScene = SCENE_MAIN_STAGE;
    return true;
}

void Cleanup(GameState *gs)
{
    // テクスチャの解放
    SDL_DestroyTexture(gs->player.texture);
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].texture)
            SDL_DestroyTexture(gs->veggies[i].texture);
    }
    // 敵のテクスチャは共有しているので一度だけ解放
    if (gs->enemies[0].texture)
        SDL_DestroyTexture(gs->enemies[0].texture);

    // フォントとサウンドの解放
    TTF_CloseFont(gs->font);
    Mix_FreeChunk(gs->damageSound);

    // SDLサブシステムの終了
    if (gs->ddrMat)
        SDL_JoystickClose(gs->ddrMat);
    SDL_DestroyRenderer(gs->renderer);
    SDL_DestroyWindow(gs->window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}