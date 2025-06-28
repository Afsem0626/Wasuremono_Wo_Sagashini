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

bool InitGame(GameState *gs)
{
    // SDLの初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 ||
        IMG_Init(IMG_INIT_PNG) == 0 ||
        TTF_Init() == -1)
    {
        fprintf(stderr, "ライブラリの初期化に失敗しました: %s\n", SDL_GetError());
        return false;
    }

    // ウィンドウ作成... (ステップ3と同じ)
    gs->window = SDL_CreateWindow("おさんぽ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!gs->window)
        return false;

    // レンダラー作成... (ステップ3と同じ)
    gs->renderer = SDL_CreateRenderer(gs->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gs->renderer)
        return false;

    // ジョイスティック初期化... (ステップ3と同じ)
    if (SDL_NumJoysticks() > 0)
    {
        gs->ddrMat = SDL_JoystickOpen(0);
    }

    // アセットの読み込み
    gs->font = TTF_OpenFont("ZenOldMincho-Bold.ttf", 28);
    if (!gs->font)
    {
        fprintf(stderr, "フォントをロードできませんでした: %s\n", TTF_GetError());
        return false;
    }

    gs->player.texture = LoadTexture("player.png", gs->renderer);
    SDL_Texture *carrotTex = LoadTexture("vegetables/carrot.png", gs->renderer);
    SDL_Texture *eggplantTex = LoadTexture("vegetables/eggplant.png", gs->renderer);
    SDL_Texture *tomatoTex = LoadTexture("vegetables/tomato.png", gs->renderer);
    if (!gs->player.texture || !carrotTex || !eggplantTex || !tomatoTex)
        return false;

    // プレイヤーと野菜の初期化
    gs->player.rect = (SDL_Rect){100, 490, 100, 100}; // プレイヤーの初期位置とサイズ
    gs->veggiesCollected = 0;

    gs->veggies[0] = (GameObject){true, {400, 500, 80, 80}, carrotTex};
    gs->veggies[1] = (GameObject){true, {900, 300, 80, 80}, eggplantTex};
    gs->veggies[2] = (GameObject){true, {1400, 600, 80, 80}, tomatoTex};

    gs->isRunning = true;
    return true;
}

// Cleanup関数も修正が必要（テクスチャとフォントの解放）
void Cleanup(GameState *gs)
{
    // テクスチャの解放
    SDL_DestroyTexture(gs->player.texture);
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        SDL_DestroyTexture(gs->veggies[i].texture);
    }
    // フォントの解放
    TTF_CloseFont(gs->font);

    // SDLサブシステムの終了
    if (gs->ddrMat)
        SDL_JoystickClose(gs->ddrMat);
    if (gs->renderer)
        SDL_DestroyRenderer(gs->renderer);
    if (gs->window)
        SDL_DestroyWindow(gs->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}