#include "draw.h"
#include <stdio.h> // sprintf用

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

void DrawGame(GameState *gs)
{
    // 背景を黒でクリア
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);

    // 野菜を描画 (アクティブなもののみ)
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive)
        {
            SDL_RenderCopy(gs->renderer, gs->veggies[i].texture, NULL, &gs->veggies[i].rect);
        }
    }

    // プレイヤーを描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);

    // UI(取得数)を描画
    char scoreText[32];
    sprintf(scoreText, "やさい: %d / %d", gs->veggiesCollected, MAX_VEGGIES);
    DrawText(gs->renderer, gs->font, scoreText, 20, 20);

    // 画面に反映
    SDL_RenderPresent(gs->renderer);
}