#include "logic.h"
#include "sound.h" // sound.hをインクルード

const int PLAYER_SPEED = 8;

// 新しい関数：敵を動かす
void UpdateEnemies(GameState *gs)
{
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive)
        {
            gs->enemies[i].rect.x += gs->enemies[i].vx;
            // 画面の左端まで行ったら右端から再出現
            if (gs->enemies[i].rect.x + gs->enemies[i].rect.w < 0)
            {
                gs->enemies[i].rect.x = screen_w;
            }
        }
    }
}

// 新しい関数：当たり判定の処理
void CheckCollisions(GameState *gs)
{
    // 野菜との当たり判定
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive && SDL_HasIntersection(&gs->player.rect, &gs->veggies[i].rect))
        {
            gs->veggies[i].isActive = false;
            gs->veggiesCollected++;
        }
    }

    // 敵との当たり判定
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive && SDL_HasIntersection(&gs->player.rect, &gs->enemies[i].rect))
        {
            gs->player.hp--;                 // HPを減らす
            gs->enemies[i].isActive = false; // 敵は一度当たったら消える
            PlaySound(gs->damageSound);      // ダメージ音を再生
            printf("ダメージ！ 残りHP: %d\n", gs->player.hp);
        }
    }
}

// UpdateGameを修正
void UpdateGame(GameState *gs, const InputState *input)
{
    // ゲームプレイ中のみ更新
    if (gs->currentScene == SCENE_MAIN_STAGE)
    {
        // プレイヤー移動
        if (input->up)
            gs->player.rect.y -= PLAYER_SPEED;
        if (input->down)
            gs->player.rect.y += PLAYER_SPEED;
        if (input->left)
            gs->player.rect.x -= PLAYER_SPEED;
        if (input->right)
            gs->player.rect.x += PLAYER_SPEED;

        // 敵移動
        UpdateEnemies(gs);

        // 当たり判定
        CheckCollisions(gs);

        // ゲームオーバー判定
        if (gs->player.hp <= 0)
        {
            gs->currentScene = SCENE_GAME_OVER;
            printf("ゲームオーバー！\n");
        }
    }
}