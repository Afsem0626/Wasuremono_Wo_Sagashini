#include "init.h"  // GameState, Playerなどの定義を含む
#include "input.h" // InputStateの定義とHandleInputの宣言を含む
#include "logic.h" // UpdateGameの宣言を含む
#include "draw.h"  // DrawGameの宣言を含む

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
        HandleInput(&gameState, &inputState);

        // 2. ロジック更新：inputStateを基に、gameStateを更新する
        UpdateGame(&gameState, &inputState);

        // 3. 描画：現在のgameStateを基に画面を描く
        DrawGame(&gameState);

        // 負荷軽減
        SDL_Delay(16);
    }

    // 終了処理
    Cleanup(&gameState);

    return 0;
}