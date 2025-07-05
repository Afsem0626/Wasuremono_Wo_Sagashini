#include "draw.h"
#include <stdio.h>

// --- このファイル内だけで使う「静的関数」の前方宣言 ---
static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);
static void DrawTitleScene(GameState *gs);
static void DrawMainStage(GameState *gs);
static void DrawGameOverScene(GameState *gs);
static void DrawVeggieMinigame(GameState *gs);
static void DrawArrowMinigame(GameState *gs);
static void DrawHUD(GameState *gs);

// ==========================================================
// 公開関数 (main.c から呼び出される司令塔)
// ==========================================================
void DrawGame(GameState *gs)
{
    // 背景をクリア
    SDL_SetRenderDrawColor(gs->renderer, 200, 200, 220, 255); // 明るい灰色
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
    case SCENE_NOVEL:
    case SCENE_ENDING:
        // 未実装
        break;
    }
    SDL_RenderPresent(gs->renderer);
}

// ==========================================================
// 以下はこのファイル内だけで使われる静的（static）関数
// ==========================================================

static void DrawTitleScene(GameState *gs)
{
    if (gs->titleTexture != NULL)
    {
        SDL_RenderCopy(gs->renderer, gs->titleTexture, NULL, NULL);
    }
    else
    {
        DrawText(gs->renderer, gs->font, "タイトル画像を読み込めませんでした", 500, 500);
    }
}

static void DrawGameOverScene(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 80, 0, 0, 255); // 暗い赤色
    SDL_RenderClear(gs->renderer);
    DrawText(gs->renderer, gs->font, "GAME OVER", 760, 490);
}

static void DrawMainStage(GameState *gs)
{
    // 現在のミニゲームに応じて描画内容を切り替える
    if (gs->currentMinigame == MINIGAME_VEGGIE)
    {
        DrawVeggieMinigame(gs);
    }
    else if (gs->currentMinigame == MINIGAME_ARROWS)
    {
        DrawArrowMinigame(gs);
    }
}

static void DrawVeggieMinigame(GameState *gs)
{
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
    // 扉を描画
    if (gs->door.doorState == DOOR_LOCKED)
    {
        SDL_RenderCopy(gs->renderer, gs->doorLockedTexture, NULL, &gs->door.rect);
    }
    else if (gs->door.doorState == DOOR_UNLOCKED)
    {
        SDL_RenderCopy(gs->renderer, gs->doorUnlockedTexture, NULL, &gs->door.rect);
    }
    // プレイヤーとHUDを描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);
    DrawHUD(gs);
}

// ★★★ アニメーション描画に対応した DrawArrowMinigame 関数 ★★★
static void DrawArrowMinigame(GameState *gs)
{
    int base_start_x = (1920 - (MAX_ARROWS * 150)) / 2;
    int y = 500;
    int size = 128;
    int spacing = 150;

    // アニメーションの進行度 (0.0から1.0) を計算
    float progress = 0.0f;
    if (gs->isArrowAnimating)
    {
        progress = gs->arrowAnimationTimer / ARROW_ANIMATION_DURATION;
        if (progress > 1.0f)
            progress = 1.0f;
    }

    // 画面に表示すべき矢印（まだクリアしていない矢印）を描画
    for (int i = gs->arrowPlayerProgress; i < MAX_ARROWS; i++)
    {
        SDL_Texture *tex = gs->arrowTextures[gs->arrowSequence[i]];

        // テクスチャの修飾をリセット
        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);

        // 基本の位置を計算
        int original_x = base_start_x + (i - gs->arrowPlayerProgress) * spacing;
        SDL_Rect destRect = {original_x, y, size, size};

        // もしアニメーション中なら、位置や透明度を上書きする
        if (gs->isArrowAnimating)
        {
            // 1. 今まさに消えつつある矢印 (iが進捗と同じ=最初の矢印)
            if (i == gs->arrowPlayerProgress)
            {
                // だんだん透明にする (アルファ値を変更)
                SDL_SetTextureAlphaMod(tex, 255 * (1.0f - progress));
            }
            // 2. 左にスライドしてくる後続の矢印
            else
            {
                destRect.x -= spacing * progress; // progressに応じて左にずらす
            }
        }
        SDL_RenderCopy(gs->renderer, tex, NULL, &destRect);
    }

    // プレイヤーとHUDも描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);
    DrawHUD(gs);
}

static void DrawHUD(GameState *gs)
{
    char hudText[128];
    sprintf(hudText, "HP: %d | 野菜: %d / %d | 時間: %.0f",
            gs->player.hp, gs->veggiesCollected, gs->veggiesRequired, gs->stageTimer);
    DrawText(gs->renderer, gs->font, hudText, 20, 20);
}

static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    if (!surface)
        return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (!texture)
        return;
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}