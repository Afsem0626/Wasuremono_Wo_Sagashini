#include "logic.h"
#include "sound.h"
#include <stdio.h>

const int PLAYER_SPEED = 20;

// --- このファイル内だけで使う「静的関数」の前方宣言 ---
static void UpdatePlayer(GameState *gs, const InputState *input); // ★★★ これを追加 ★★★
static void UpdateEnemies(GameState *gs);
static void CheckCollisions(GameState *gs);
static void UpdateTitleScene(GameState *gs, const InputState *input);
static void UpdateMainStage(GameState *gs, const InputState *input);
static void UpdateGameOverScene(GameState *gs, const InputState *input);
static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b);
static void ResetStage(GameState *gs);

// ==========================================================
// 司令塔となる公開関数
// ==========================================================
void UpdateGame(GameState *gs, const InputState *input)
{
    switch (gs->currentScene)
    {
    case SCENE_TITLE:
        UpdateTitleScene(gs, input);
        break;
    case SCENE_MAIN_STAGE:
        UpdateMainStage(gs, input);
        break;
    case SCENE_GAME_OVER:
        UpdateGameOverScene(gs, input);
        break;
    }
}

// ==========================================================
// 以下はこのファイル内だけで使われる静的（static）関数
// ==========================================================

static void UpdateMainStage(GameState *gs, const InputState *input)
{
    // ★★★ プレイヤーの移動処理を、専用の関数呼び出しに変更 ★★★
    UpdatePlayer(gs, input);

    UpdateEnemies(gs);
    CheckCollisions(gs);

    if (gs->player.hp <= 0)
    {
        gs->currentScene = SCENE_GAME_OVER;
        printf("ゲームオーバー！\n");
    }
}

// ★★★ 新しく追加するプレイヤー移動関数 ★★★
static void UpdatePlayer(GameState *gs, const InputState *input)
{
    if (input->up_held)
        gs->player.rect.y -= PLAYER_SPEED;
    if (input->down_held)
        gs->player.rect.y += PLAYER_SPEED;
    if (input->left_held)
        gs->player.rect.x -= PLAYER_SPEED;
    if (input->right_held)
        gs->player.rect.x += PLAYER_SPEED;
}

// --- 以下、他の静的関数 (前回から変更なし) ---

static void UpdateTitleScene(GameState *gs, const InputState *input)
{
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed)
    {
        ResetStage(gs);
        gs->currentScene = SCENE_MAIN_STAGE;
    }
}

static void UpdateGameOverScene(GameState *gs, const InputState *input)
{
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed)
    {
        gs->currentScene = SCENE_TITLE;
    }
}

static void UpdateEnemies(GameState *gs)
{
    int screen_w;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, NULL);
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive)
        {
            gs->enemies[i].rect.x += gs->enemies[i].vx;
            if (gs->enemies[i].rect.x + gs->enemies[i].rect.w < 0)
            {
                gs->enemies[i].rect.x = screen_w;
            }
        }
    }
}

static void CheckCollisions(GameState *gs)
{
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive && DetectCollision(&gs->player.rect, &gs->veggies[i].rect))
        {
            gs->veggies[i].isActive = false;
            gs->veggiesCollected++;
        }
    }
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive && DetectCollision(&gs->player.rect, &gs->enemies[i].rect))
        {
            gs->player.hp--;
            gs->enemies[i].isActive = false;
            PlaySound(gs->damageSound);
            printf("ダメージ！ 残りHP: %d\n", gs->player.hp);
        }
    }
}

static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b)
{
    return SDL_HasIntersection(a, b);
}

static void ResetStage(GameState *gs)
{
    // ... (ResetStageの中身は変更なし) ...
}