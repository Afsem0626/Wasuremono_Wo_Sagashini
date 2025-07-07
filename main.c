#include "init.h"
#include "input.h"
#include "logic.h"
#include "draw.h"

int main(int argc, char *argv[])
{
    // GameStateとInputStateを別々の変数として用意する
    GameState gameState = {0};
    InputState inputState = {0};

    // ゲームの初期化
    if (!InitGame(&gameState))
    {
        return 1;
    }
    // アセットの読み込みも呼び出す
    LoadAssets(&gameState);

    // メインループ
    gameState.isRunning = true;
    while (gameState.isRunning)
    {

        // 修正前
        // HandleInput(&inputState, &gameState.isRunning);

        // 修正後
        // 入力処理
        HandleInput(&gameState, &inputState);

        // ロジック更新：inputStateを基に、gameStateを更新する
        UpdateGame(&gameState, &inputState);

        // 描画：現在のgameStateを元に画面を描く
        DrawGame(&gameState);

        // 負荷軽減のためにディレイをかけるぜ
        SDL_Delay(16);
    }

    // 終了処理
    Cleanup(&gameState);

    return 0;
}