#include "draw.h"
#include <stdio.h>

// ... DrawText関数は変更なし ...

// 新しい関数：ゲームオーバー画面を描画
void DrawGameOverScene(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 80, 0, 0, 255); // 暗い赤色
    SDL_RenderClear(gs->renderer);
    DrawText(gs->renderer, gs->font, "GAME OVER", 760, 490);
}

// 新しい関数：メインステージを描画
void DrawMainStage(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255); // 背景を黒に
    SDL_RenderClear(gs->renderer);

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

    // プレイヤーを描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);

    // UIを描画
    char hudText[64];
    sprintf(hudText, "やさい: %d / %d   HP: %d", gs->veggiesCollected, MAX_VEGGIES, gs->player.hp);
    DrawText(gs->renderer, gs->font, hudText, 20, 20);
}

// DrawGameを修正
void DrawGame(GameState *gs)
{
    // 現在のシーンに応じて描画を切り替える
    switch (gs->currentScene)
    {
    case SCENE_MAIN_STAGE:
        DrawMainStage(gs);
        break;
    case SCENE_GAME_OVER:
        DrawGameOverScene(gs);
        break;
    }
    SDL_RenderPresent(gs->renderer);
}