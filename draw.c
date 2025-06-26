#include "draw.h"

void DrawGame(GameState *gs)
{
    // 背景を黒でクリア
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);

    // プレイヤーを白で描画
    SDL_SetRenderDrawColor(gs->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gs->renderer, &gs->player.rect);

    // 画面に反映
    SDL_RenderPresent(gs->renderer);
}