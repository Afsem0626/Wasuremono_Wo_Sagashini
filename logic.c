#include "logic.h"

const int PLAYER_SPEED = 8;

void UpdateGame(GameState *gs)
{
    // 入力状態に応じてプレイヤーの座標を更新する
    if (gs->input.up)
    {
        gs->player.rect.y -= PLAYER_SPEED;
    }
    if (gs->input.down)
    {
        gs->player.rect.y += PLAYER_SPEED;
    }
    if (gs->input.left)
    {
        gs->player.rect.x -= PLAYER_SPEED;
    }
    if (gs->input.right)
    {
        gs->player.rect.x += PLAYER_SPEED;
    }

    // 画面外に出ないようにする処理 (あとで追加)
}