#ifndef LOGIC_H
#define LOGIC_H

#include "structs.h" // GameStateやInputStateの定義を読み込む

// 関数プロトタイプ宣言
void UpdateGame(GameState *gs, const InputState *input);
void UpdateEnemies(GameState *gs);
void CheckCollisions(GameState *gs);

#endif // LOGIC_H