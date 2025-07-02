#include "logic.h"
#include "sound.h" // sound.hをインクルード

const int PLAYER_SPEED = 20;

static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b);
static void ResetStage(GameState *gs);
static void UpdateEnemies(GameState *gs);
static void CheckCollisions(GameState *gs);
static void UpdateTitleScene(GameState *gs, const InputState *input);
static void UpdateMainStage(GameState *gs, const InputState *input);
static void UpdateGameOverScene(GameState *gs, const InputState *input);

// 新しい関数：敵を動かす
static void UpdateEnemies(GameState *gs)
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
static void CheckCollisions(GameState *gs)
{
    // 野菜との当たり判定
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive && DetectCollision(&gs->player.rect, &gs->veggies[i].rect))
        {
            gs->veggies[i].isActive = false;
            gs->veggiesCollected++;
        }
    }

    // 敵との当たり判定
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive && DetectCollision(&gs->player.rect, &gs->enemies[i].rect))
        {
            gs->player.hp--;                 // HPを減らす
            gs->enemies[i].isActive = false; // 敵は一度当たったら消える
            PlaySound(gs->damageSound);      // ダメージ音を再生
            printf("ダメージ！ 残りHP: %d\n", gs->player.hp);
        }
    }
}

// UpdateGameを修正
static void UpdateMainStage(GameState *gs, const InputState *input)
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

// --- 司令塔となるUpdateGame関数 ---
void UpdateGame(GameState *gs, const InputState *input)
{
    // 現在のシーンに応じて、適切な更新関数を呼び出す
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
        // case SCENE_NOVEL: など、他のシーンもここに追加
    }
}

static void UpdateGameOverScene(GameState *gs, const InputState *input)
{
    // いずれかのパネルが「押された瞬間」なら、タイトル画面に戻る
    if (input->up || input->down || input->left || input->right)
    {
        gs->currentScene = SCENE_TITLE;
    }
}

// --- 内部でのみ使用する補助関数 ---
static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b)
{
    return SDL_HasIntersection(a, b);
}

// ★★★ 新規追加 ★★★：ステージの状態をリセットする関数
static void ResetStage(GameState *gs)
{
    printf("ステージをリセットします。\n");
    gs->player.hp = 5; // HPを初期値に戻す
    gs->veggiesCollected = 0;

    // 野菜の位置と状態をリセット
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        gs->veggies[i].isActive = true;
        // ここで野菜の初期位置を再設定するロジックを追加
        gs->veggies[i].rect.x = 400 + i * 500;
        gs->veggies[i].rect.y = 300 + (i % 2) * 200;
    }
    // 敵の位置と状態も同様にリセット
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true;
        // ここで敵の初期位置を再設定するロジックを追加
        int screen_w, screen_h;
        SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
        gs->enemies[i].rect.x = screen_w + (i * 400);
    }
}

// タイトル画面のロジック
static void UpdateTitleScene(GameState *gs, const InputState *input)
{
    // いずれかのパネルが「押された瞬間」なら、ステージをリセットしてゲーム開始
    if (input->up || input->down || input->left || input->right)
    {
        ResetStage(gs); // ゲーム開始前に状態をリセット
        gs->currentScene = SCENE_MAIN_STAGE;
        gs->currentMinigame = MINIGAME_ARROWS; // ★★★ テストのため、ミニゲーム2を強制的に開始 ★★★
    }
}