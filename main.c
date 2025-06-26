#include "init.h"
#include "input.h"
#include "draw.h"
#include "logic.h"

int main(int argc, char *argv[])
{
    GameState gameState = {0}; // ゲーム状態を初期化

    // ゲームの初期化
    if (!InitGame(&gameState))
    {
        return 1; // 初期化に失敗したら終了
    }

    while (gameState.isRunning)
    {

        HandleInput(&gameState); // 1. 入力処理
        UpdateGame(&gameState);  // 2. ロジック更新
        DrawGame(&gameState);    // 3. 描画

        SDL_Delay(16);
    }
    // 終了処理
    Cleanup(&gameState);

    return 0;
}