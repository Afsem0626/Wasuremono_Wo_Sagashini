#include "draw.h"
#include <stdio.h>

// --- このファイル内だけで使う「静的関数」の前方宣言 ---
static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);
static void DrawTitleScene(GameState *gs);
static void DrawMainStage(GameState *gs);
static void DrawStageClearScene(GameState *gs);
static void DrawGameOverScene(GameState *gs);
static void DrawEndingScene(GameState *gs);
static void DrawVeggieMinigame(GameState *gs);
static void DrawArrowMinigame(GameState *gs);
static void DrawHUD(GameState *gs);

// 公開関数 (main.c から呼び出される関数)
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
    case SCENE_DIFFICULTY:
        DrawDifficultyScene(gs);
        break;
    case SCENE_MAIN_STAGE:
        DrawMainStage(gs);
        break;
    case SCENE_STAGE_CLEAR: // ★★★ 追加 ★★★
        DrawStageClearScene(gs);
        break;
    case SCENE_GAME_OVER:
        DrawGameOverScene(gs);
        break;
    case SCENE_NOVEL:
        // 未実装
        break;
    case SCENE_ENDING:
        DrawEndingScene(gs);
        break;
    }
    SDL_RenderPresent(gs->renderer);
}

void DrawDifficultyScene(GameState *gs)
{
    // 難易度の選択肢を文字列の配列として定義
    const char *difficulty_names[] = {
        "昼 (Easy)",
        "夕方 (Normal)",
        "深夜 (Hard)",
        "異空 (Expert)"};

    // 画面をクリア（背景色）
    SDL_SetRenderDrawColor(gs->renderer, 30, 30, 50, 255); // 深い青色
    SDL_RenderClear(gs->renderer);

    // ヘッダーテキストを描画
    DrawText(gs->renderer, gs->font, "難易度を選んでください", 650, 200);

    // 各選択肢をループで描画
    for (int i = 0; i < DIFFICULTY_COUNT; i++)
    {
        // 現在選択されている項目かどうかを判定
        if (i == gs->difficultySelection)
        {
            // DrawText関数を一時的に改造するか、色を引数で渡せるようにするとより良い
            // ここでは簡易的に、選択されている項目を示すカーソルを描画する
            DrawText(gs->renderer, gs->font, "→", 700, 400 + i * 100);
            DrawText(gs->renderer, gs->font, difficulty_names[i], 750, 400 + i * 100);
        }
        else
        {
            // 選択されていない項目は通常の色で描画
            DrawText(gs->renderer, gs->font, difficulty_names[i], 750, 400 + i * 100);
        }
    }
}

// 以下はこのファイル内だけで使われる静的（static）関数

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

// カットイン用
static void DrawStageClearScene(GameState *gs)
{
    // まず、背景としてメインステージの画面をそのまま描画する
    DrawMainStage(gs);

    // その上に、半透明の黒いフィルターをかける
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 128); // 128は透明度
    SDL_RenderFillRect(gs->renderer, NULL);             // 画面全体を覆う

    // 「STAGE CLEAR」の文字を中央に大きく描画
    DrawText(gs->renderer, gs->largeFont, "STAGE CLEAR!", 600, 450);
}

static void DrawEndingScene(GameState *gs)
{
    // 背景を明るいクリーム色でクリア
    SDL_SetRenderDrawColor(gs->renderer, 250, 250, 210, 255);
    SDL_RenderClear(gs->renderer);

    DrawText(gs->renderer, gs->font, "GAME CLEAR", 760, 400);
    DrawText(gs->renderer, gs->font, "Congratulations!", 780, 500);
    DrawText(gs->renderer, gs->font, "パネルをふんでタイトルへ", 700, 700);
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

    // 矢印（矢印）を描画
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

    sprintf(hudText, "ステージ: %d / %d | HP: %d | 野菜: %d / %d | 時間: %.0f",
            gs->minigamesCleared + 1, // 現在のステージ数
            gs->minigamesRequired,    // 目標ステージ数
            gs->player.hp,
            gs->veggiesCollected,
            gs->veggiesRequired,
            gs->stageTimer);

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