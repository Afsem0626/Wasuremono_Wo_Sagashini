#include "init.h"
#include <stdio.h>

bool InitGame(GameState *gs)
{
    // SDLの初期化（ビデオとジョイスティック）
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        fprintf(stderr, "SDL初期化エラー: %s\n", SDL_GetError());
        return false;
    }

    // フルスクリーンウィンドウの作成
    gs->window = SDL_CreateWindow("忘れ物を探シニ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!gs->window)
    {
        fprintf(stderr, "ウィンドウ作成エラー: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // レンダラーの作成
    gs->renderer = SDL_CreateRenderer(gs->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gs->renderer)
    {
        fprintf(stderr, "レンダラー作成エラー: %s\n", SDL_GetError());
        SDL_DestroyWindow(gs->window);
        SDL_Quit();
        return false;
    }

    // ジョイスティックの初期化
    if (SDL_NumJoysticks() < 1)
    {
        fprintf(stderr, "警告: ジョイスティックが見つかりません。\n");
        gs->ddrMat = NULL;
    }
    else
    {
        gs->ddrMat = SDL_JoystickOpen(0); // 0番目のジョイスティックを開く
        if (!gs->ddrMat)
        {
            fprintf(stderr, "ジョイスティックを開けませんでした: %s\n", SDL_GetError());
        }
    }

    // init.c の InitGame 関数の最後に、gs->isRunning = true; の前に追加

    // プレイヤーの初期化
    gs->player.rect.w = 50;
    gs->player.rect.h = 50;

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->player.rect.x = (screen_w - gs->player.rect.w) / 2;
    gs->player.rect.y = (screen_h - gs->player.rect.h) / 2;

    gs->isRunning = true;
    printf("初期化が完了しました。\n");
    return true;
}

void Cleanup(GameState *gs)
{
    if (gs->ddrMat)
    {
        SDL_JoystickClose(gs->ddrMat);
    }
    if (gs->renderer)
    {
        SDL_DestroyRenderer(gs->renderer);
    }
    if (gs->window)
    {
        SDL_DestroyWindow(gs->window);
    }
    SDL_Quit();
    printf("リソースを解放し、終了しました。\n");
}