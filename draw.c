#include "draw.h"
#include <stdio.h> // sprintf用

static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);
static void DrawTitleScene(GameState *gs);
static void DrawArrowMinigame(GameState *gs);
static void DrawMainStage(GameState *gs);
static void DrawGameOverScene(GameState *gs);
static void DrawHUD(GameState *gs); // ★★★ この行を追加 ★★★

// UI(文字)描画の補助関数
void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255}; // 白色
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// ゲームオーバー画面を描画
void DrawGameOverScene(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 80, 0, 0, 255); // 暗い赤色
    SDL_RenderClear(gs->renderer);
    DrawText(gs->renderer, gs->font, "GAME OVER", 760, 490);
}

// ミニゲーム1を描画
void DrawMainStage(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 200, 200, 220, 255); // 背景を黒に
    SDL_RenderClear(gs->renderer);

    if (gs->currentMinigame == MINIGAME_VEGGIE)
    {
        printf(" -> Drawing Veggie Minigame.\n");
        // 野菜を描画
        for (int i = 0; i < MAX_VEGGIES; i++)
        {
            if (gs->veggies[i].isActive)
            {
                SDL_RenderCopy(gs->renderer, gs->veggies[i].texture, NULL, &gs->veggies[i].rect);
            }
        }

        // 敵を描画
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (gs->enemies[i].isActive)
            {
                SDL_RenderCopy(gs->renderer, gs->enemies[i].texture, NULL, &gs->enemies[i].rect);
            }
        }
        // ★★★ 扉の状態に応じてテクスチャを描き分ける ★★★
        if (gs->door.doorState == DOOR_LOCKED)
        {
            SDL_RenderCopy(gs->renderer, gs->doorLockedTexture, NULL, &gs->door.rect);
        }
        else if (gs->door.doorState == DOOR_UNLOCKED)
        {
            SDL_RenderCopy(gs->renderer, gs->doorUnlockedTexture, NULL, &gs->door.rect);
        }

        DrawHUD(gs);
    }
    else if (gs->currentMinigame == MINIGAME_ARROWS)
    {
        printf(" -> Drawing Arrow Minigame.\n");
        DrawArrowMinigame(gs);
    }

    // プレイヤーを描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);

    // UIを描画
    char hudText[64];
    sprintf(hudText, "回収した野菜: %d / %d   HP: %d", gs->veggiesCollected, MAX_VEGGIES, gs->player.hp);
    DrawText(gs->renderer, gs->font, hudText, 20, 20);
}

// ミニゲーム2（矢印）を描画
void DrawArrowMinigame(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 200, 200, 100, 255); // 背景を黄色
    SDL_RenderClear(gs->renderer);
    // 画面中央に矢印を並べる
    int start_x = (1920 - (MAX_ARROWS * 150)) / 2; // 1920は画面幅、150は矢印の幅+間隔
    int y = 500;
    int size = 128; // 矢印のサイズ

    for (int i = 0; i < MAX_ARROWS; i++)
    {
        SDL_Rect destRect = {start_x + i * 150, y, size, size};
        SDL_Texture *tex = gs->arrowTextures[gs->arrowSequence[i]];

        // 正解済みの矢印は緑色にする
        if (i < gs->arrowPlayerProgress)
        {
            SDL_SetTextureColorMod(tex, 100, 255, 100); // 緑色
        }
        else
        {
            SDL_SetTextureColorMod(tex, 255, 255, 255); // 元の色
        }

        SDL_RenderCopy(gs->renderer, tex, NULL, &destRect);
    }
}

void DrawGame(GameState *gs)
{
    // 背景をクリア
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);
    // 現在のシーンに応じて描画を切り替える
    switch (gs->currentScene)
    {
    case SCENE_TITLE:
        DrawTitleScene(gs);
        break;
    case SCENE_MAIN_STAGE:
        DrawMainStage(gs);
        break;
    case SCENE_GAME_OVER:
        DrawGameOverScene(gs);
        break;
    case SCENE_NOVEL: // ★★★ この行を追加 ★★★
        // 今はまだ何もしない
        break;
    }
    SDL_RenderPresent(gs->renderer);
}

void DrawTitleScene(GameState *gs)
{
    DrawText(gs->renderer, gs->font, "忘れ物を探シニ", 960, 400);
    DrawText(gs->renderer, gs->font, "パネルをふんで はじめる", 960, 600);
}

static void DrawHUD(GameState *gs)
{
    // HPやスコア、時間を描画するロジック
    char hudText[128];
    sprintf(hudText, "HP: %d | 野菜: %d / %d | 時間: %.0f",
            gs->player.hp, gs->veggiesCollected, gs->veggiesRequired, gs->stageTimer);

    // DrawText関数は既に定義されているので、それを使って描画
    DrawText(gs->renderer, gs->font, hudText, 20, 20);
}