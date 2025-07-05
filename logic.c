#include "logic.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int PLAYER_SPEED = 20;

// --- このファイル内だけで使う「静的関数」の前方宣言 ---
static void UpdateTitleScene(GameState *gs, const InputState *input);
static void UpdateGameOverScene(GameState *gs, const InputState *input);
static void UpdateVeggieMinigame(GameState *gs, const InputState *input);
static void UpdateArrowMinigame(GameState *gs, const InputState *input);
static void StartNewRandomMinigame(GameState *gs);
static void ResetStage(GameState *gs);
static void UpdatePlayer(GameState *gs, const InputState *input);
static void UpdateEnemies(GameState *gs);
static void CheckCollisions(GameState *gs);
static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b);

// ==========================================================
// 公開関数 (main.c から呼び出される司令塔)
// ==========================================================
void UpdateGame(GameState *gs, const InputState *input)
{
    switch (gs->currentScene)
    {
    case SCENE_TITLE:
        UpdateTitleScene(gs, input);
        break;
    case SCENE_MAIN_STAGE:
        // メインステージの中では、さらにミニゲームの種類で処理を分ける
        if (gs->currentMinigame == MINIGAME_VEGGIE)
        {
            UpdateVeggieMinigame(gs, input);
        }
        else if (gs->currentMinigame == MINIGAME_ARROWS)
        {
            UpdateArrowMinigame(gs, input);
        }
        break;
    case SCENE_GAME_OVER:
        UpdateGameOverScene(gs, input);

    case SCENE_NOVEL: // ★★★ この行を追加 ★★★
        // 今はまだ何もしない
        break;
    case SCENE_ENDING:
        // UpdateEndingScene(gs, input); // 後で作成するエンディング用の更新関数を呼ぶ
        break;
    }
}

// ==========================================================
// 以下はこのファイル内だけで使われる静的（static）関数
// ==========================================================

static void UpdateTitleScene(GameState *gs, const InputState *input)
{
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed)
    {
        StartNewRandomMinigame(gs);
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

static void UpdateVeggieMinigame(GameState *gs, const InputState *input)
{
    UpdatePlayer(gs, input);
    UpdateEnemies(gs);
    CheckCollisions(gs);

    if (gs->veggiesCollected >= gs->veggiesRequired && gs->door.doorState == DOOR_LOCKED)
    {
        printf("全ての野菜を回収！ 扉のロックが解除された！\n");
        gs->door.doorState = DOOR_UNLOCKED;
        // ここでロック解除の効果音を鳴らすと良い
    }
    if (gs->player.hp <= 0)
        gs->currentScene = SCENE_GAME_OVER;
}

static void UpdateArrowMinigame(GameState *gs, const InputState *input)
{
    int expected_input = gs->arrowSequence[gs->arrowPlayerProgress];
    bool correct_input = false;
    bool wrong_input = false;

    if (input->up_pressed)
    {
        if (expected_input == ARROW_UP)
            correct_input = true;
        else
            wrong_input = true;
    }
    if (input->down_pressed)
    {
        if (expected_input == ARROW_DOWN)
            correct_input = true;
        else
            wrong_input = true;
    }
    if (input->left_pressed)
    {
        if (expected_input == ARROW_LEFT)
            correct_input = true;
        else
            wrong_input = true;
    }
    if (input->right_pressed)
    {
        if (expected_input == ARROW_RIGHT)
            correct_input = true;
        else
            wrong_input = true;
    }

    if (correct_input)
    {
        gs->arrowPlayerProgress++;
        if (gs->arrowPlayerProgress >= MAX_ARROWS)
        {
            printf("ミニゲーム2 クリア！\n");
            gs->minigamesCleared++; // クリア数を1増やす
            if (gs->minigamesCleared >= gs->minigamesRequired)
            {
                gs->currentScene = SCENE_ENDING; // 目標数に達したらエンディングへ
                printf("ゲームクリア！\n");
            }
            else
            {
                StartNewRandomMinigame(gs); // まだなら次のミニゲームへ
            }
        }
    }
    else if (wrong_input)
    {
        // gs->player.hp--;
        // PlaySound(gs->damageSound);
        gs->arrowPlayerProgress = 0;
    }
    if (gs->player.hp <= 0)
        gs->currentScene = SCENE_GAME_OVER;
}

static void StartNewRandomMinigame(GameState *gs)
{
    ResetStage(gs);
    if (rand() % 2 == 0)
    {
        gs->currentMinigame = MINIGAME_VEGGIE;
        printf("次のミニゲーム: 野菜集め\n");
    }
    else
    {
        gs->currentMinigame = MINIGAME_ARROWS;
        printf("次のミニゲーム: 矢印入力\n");
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

    if (gs->door.doorState == DOOR_UNLOCKED && DetectCollision(&gs->player.rect, &gs->door.rect))
    {
        printf("扉に入った！ ミニゲーム1 クリア！\n");
        gs->minigamesCleared++; // クリア数を1増やす
        if (gs->minigamesCleared >= gs->minigamesRequired)
        {
            gs->currentScene = SCENE_ENDING; // 目標数に達したらエンディングへ
            printf("ゲームクリア！\n");
        }
        else
        {
            StartNewRandomMinigame(gs); // まだなら次のミニゲームへ
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

// ★★★ 新しく追加するプレイヤー移動関数 ★★★
static void UpdatePlayer(GameState *gs, const InputState *input)
{
    // 1. これまでの移動処理
    if (input->up_held)
        gs->player.rect.y -= PLAYER_SPEED;
    if (input->down_held)
        gs->player.rect.y += PLAYER_SPEED;
    if (input->left_held)
        gs->player.rect.x -= PLAYER_SPEED;
    if (input->right_held)
        gs->player.rect.x += PLAYER_SPEED;

    // --- ★★★ ここから境界チェック処理を追加 ★★★ ---

    // 2. 画面のサイズを取得
    int screen_w, screen_h;
    // GameState構造体からrendererポインタを渡す
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);

    // 3. 左右の壁との判定
    // 左の壁
    if (gs->player.rect.x < 0)
    {
        gs->player.rect.x = 0;
    }
    // 右の壁 (プレイヤーの幅も考慮する)
    if (gs->player.rect.x + gs->player.rect.w > screen_w)
    {
        gs->player.rect.x = screen_w - gs->player.rect.w;
    }

    // 4. 上下の壁との判定
    // 上の壁
    if (gs->player.rect.y < 0)
    {
        gs->player.rect.y = 0;
    }
    // 下の壁 (プレイヤーの高さも考慮する)
    if (gs->player.rect.y + gs->player.rect.h > screen_h)
    {
        gs->player.rect.y = screen_h - gs->player.rect.h;
    }
}

static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b)
{
    return SDL_HasIntersection(a, b);
}

static void ResetStage(GameState *gs)
{
    printf("ステージをリセットします。\n");

    // ★★★ ゲーム開始時・リスタート時にクリア状況をリセット ★★★
    if (gs->currentScene == SCENE_TITLE)
    { // タイトルから始める時だけリセット
        gs->minigamesCleared = 0;
        // 難易度に応じて変更する（今は仮に4つとする）
        gs->minigamesRequired = 4;
    }

    // --- プレイヤーの状態をリセット ---
    gs->player.hp = 2;
    // クリアに必要な野菜の数を設定 (今は定数MAX_VEGGIESを使う)
    gs->veggiesRequired = MAX_VEGGIES;

    // 画面サイズを取得して、プレイヤーを初期位置（左側の中央）に配置
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->player.rect.x = 100;
    gs->player.rect.y = (screen_h - gs->player.rect.h) / 2;

    // --- 野菜の状態をリセット ---
    gs->veggiesCollected = 0;
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        gs->veggies[i].isActive = true; // 全ての野菜を有効化
        // 野菜の初期位置を再設定
        gs->veggies[i].rect.x = 600 + (rand() % 800);
        gs->veggies[i].rect.y = 150 + (rand() % 700);
    }

    // ★★★ 扉の状態を「ロックされた」状態で初期化 ★★★
    gs->door.doorState = DOOR_LOCKED;
    // 扉の初期位置を固定（例：画面右端の中央）
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->door.rect = (SDL_Rect){screen_w - 150, (screen_h - 128) / 2, 128, 128};

    // 扉を閉めたままに
    gs->door.isActive = false;

    // --- 敵の状態をリセット ---
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true; // 全ての敵を有効化
        // 敵の初期位置を画面右外に再設定
        gs->enemies[i].rect.x = screen_w + (i * 200) + (rand() % 300);
        gs->enemies[i].rect.y = 100 + (rand() % 800);
    }

    // --- ミニゲーム2（矢印入力）の状態をリセット ---
    gs->arrowPlayerProgress = 0; // プレイヤーの進捗をリセット
    // お題となる矢印シーケンスをランダムに再生成
    for (int i = 0; i < MAX_ARROWS; i++)
    {
        gs->arrowSequence[i] = rand() % 4; // 0〜3の乱数を生成
    }

    // --- 残り時間をリセット ---
    gs->stageTimer = 30.0f; // 例として30秒に設定
}