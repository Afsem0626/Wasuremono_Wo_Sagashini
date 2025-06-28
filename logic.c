#include "logic.h"

const int PLAYER_SPEED = 12;

// 当たり判定の補助関数
bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b)
{
    return SDL_HasIntersection(a, b);
}

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
    // 当たり判定
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        // アクティブな野菜との当たり判定のみ行う
        if (gs->veggies[i].isActive && DetectCollision(&gs->player.rect, &gs->veggies[i].rect))
        {
            gs->veggies[i].isActive = false; // 野菜を非アクティブに
            gs->veggiesCollected++;          // カウンタを増やす
            // ここで効果音を鳴らす処理を入れる (PlaySound()など)
        }
    }
    // 画面外に出ないようにする処理 (あとで追加)
}