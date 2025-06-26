#include "init.h"

int main(int argc, char *argv[])
{
    GameState gameState;

    // ゲームの初期化
    if (!InitGame(&gameState))
    {
        return 1; // 初期化に失敗したら終了
    }

    // 背景を黒でクリアして表示（テストのため）
    SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gameState.renderer);
    SDL_RenderPresent(gameState.renderer);

    // ウィンドウが3秒間表示されるのを確認するための待機
    printf("3秒後にウィンドウを閉じる\n");
    SDL_Delay(3000);

    // 終了処理
    Cleanup(&gameState);

    return 0;
}