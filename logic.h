#ifndef LOGIC_H
#define LOGIC_H

#include "structs.h" // GameStateやInputStateの定義を読み込む

// 外部から呼び出すのは、この司令塔関数だけで良い
void UpdateGame(GameState *gs, const InputState *input);

// 関数プロトタイプ宣言
// void UpdateMainStage(GameState *gs, const InputState *input);
// void UpdateEnemies(GameState *gs);
// void CheckCollisions(GameState *gs);

#endif // LOGIC_H