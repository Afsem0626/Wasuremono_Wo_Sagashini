#include "logic.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int PLAYER_SPEED = 20;

static void UpdateTitleScene(GameState *gs, const InputState *input);
static void UpdateDifficultyScene(GameState *gs, const InputState *input);
static void UpdateGameOverScene(GameState *gs, const InputState *input);
static void UpdateVeggieMinigame(GameState *gs, const InputState *input);
static void UpdateStageClearScene(GameState *gs);
static void UpdateArrowMinigame(GameState *gs, const InputState *input);
static void StartNewRandomMinigame(GameState *gs);
static void ResetStage(GameState *gs);
static void UpdatePlayer(GameState *gs, const InputState *input);
static void UpdateEnemies(GameState *gs);
static void CheckCollisions(GameState *gs);
static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b);

// main.c で呼び出す関数

void UpdateGame(GameState *gs, const InputState *input)
{

    switch (gs->currentScene)
    {
    case SCENE_TITLE:
        UpdateTitleScene(gs, input);
        break;
    case SCENE_DIFFICULTY: // 難易度選択
        UpdateDifficultyScene(gs, input);
        break;
    case SCENE_MAIN_STAGE:

        // タイマーを減らす処理
        gs->stageTimer -= (1.0f / 60.0f); // 60FPSを仮定

        // 時間切れでゲームオーバーになる判定
        if (gs->stageTimer <= 0)
        {
            gs->currentScene = SCENE_GAME_OVER;
            printf("時間切れ！ゲームオーバー！\n");
            break; // 時間切れなので、このフレームの他の処理は行わない
        }
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
        break;
    case SCENE_STAGE_CLEAR:
        UpdateStageClearScene(gs);
        break;

    case SCENE_NOVEL:
        // 未実装
        break;
    case SCENE_ENDING:
        // 未実装
        break;
    }
}

// 以下はこのファイル内だけで使われる静的関数

static void UpdateTitleScene(GameState *gs, const InputState *input)
{
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed)
    {
        // StartNewRandomMinigame(gs);
        gs->currentScene = SCENE_DIFFICULTY;
    }
}

static void UpdateDifficultyScene(GameState *gs, const InputState *input)
{
    // 上が押されたらカーソルを一つ上に（ループする）
    if (input->up_pressed)
    {
        gs->difficultySelection = (gs->difficultySelection - 1 + DIFFICULTY_COUNT) % DIFFICULTY_COUNT;
    }
    if (input->down_pressed)
    {
        gs->difficultySelection = (gs->difficultySelection + 1) % DIFFICULTY_COUNT;
    }

    // 決定ボタン（Aボタンなど）が押されたら
    if (input->a_pressed)
    {
        // 選択されているカーソル位置を、実際の難易度として設定
        gs->difficulty = (Difficulty)gs->difficultySelection;

        gs->minigamesCleared = 0;
        // ステージをリセットしてゲームを開始
        StartNewRandomMinigame(gs);
        gs->currentScene = SCENE_MAIN_STAGE;
        printf("難易度 %d でゲーム開始！\n", gs->difficulty);
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
    if (gs->player.hp <= 0)
        gs->currentScene = SCENE_GAME_OVER;
    if (gs->veggiesCollected >= gs->veggiesRequired && gs->door.doorState == DOOR_LOCKED)
    {
        printf("全ての野菜を回収！ 扉のロックが解除された！\n");
        gs->door.doorState = DOOR_UNLOCKED;
        // ここでロック解除の効果音を鳴らす予定
    }
}

static void UpdateArrowMinigame(GameState *gs, const InputState *input)
{
    // --- アニメーション中の処理 ---
    if (gs->isArrowAnimating)
    {
        // タイマーを進める (1/60.0fは60FPSを仮定)
        gs->arrowAnimationTimer += (1.0f / 60.0f);

        // アニメーションが完了したら
        if (gs->arrowAnimationTimer >= ARROW_ANIMATION_DURATION)
        {
            gs->isArrowAnimating = false; // アニメーション終了
            gs->arrowPlayerProgress++;    // プレイヤーの進捗をここで初めて進める

            // これでミニゲームクリアか判定
            if (gs->arrowPlayerProgress >= gs->arrowCount)
            {
                printf("ミニゲーム2 クリア！\n");
                gs->minigamesCleared++;

                if (gs->minigamesCleared >= gs->minigamesRequired)
                {
                    gs->currentScene = SCENE_ENDING;
                }
                else
                {
                    // 次のゲームに直接進むのではなく、クリア画面に移行
                    gs->currentScene = SCENE_STAGE_CLEAR;
                    gs->transitionTimer = 2.0f; // カットインを2秒間表示する
                }
            }
        }
        return; // アニメーション中は以降の入力処理を行わない
    }

    // --- 入力待ちの処理 ---
    int expected_input = gs->arrowSequence[gs->arrowPlayerProgress];
    bool correct_input = false;
    bool wrong_input = false;

    // (入力判定ロジックは同じ)
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

    // 正解だった場合の処理
    if (correct_input)
    {
        // すぐに進捗を進めるのではなく、アニメーションを開始する
        gs->isArrowAnimating = true;
        gs->arrowAnimationTimer = 0.0f; // タイマーをリセット
        // 正解の効果音を鳴らす
    }
    // 不正解だった場合の処理
    else if (wrong_input)
    {
        // gs->player.hp--; // HPは減らさない
        PlaySound(gs->damageSound);
        gs->arrowPlayerProgress = 0; // 最初からやり直し
    }

    // HPが0になったらゲームオーバー
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
            gs->player.hp--;
            gs->enemies[i].isActive = false;
            PlaySound(gs->damageSound);
            printf("ダメージ！ 残りHP: %d\n", gs->player.hp);
        }
    }
    // 扉との当たり判定
    if (gs->player.hp > 0 && gs->door.doorState == DOOR_UNLOCKED && DetectCollision(&gs->player.rect, &gs->door.rect))
    {
        printf("扉に入った！ ミニゲーム1 クリア！\n");
        gs->minigamesCleared++; // クリア数を増やす

        // 目標数に達したかチェック
        if (gs->minigamesCleared >= gs->minigamesRequired)
        {
            gs->currentScene = SCENE_ENDING; // 完全にゲームクリアならエンディングへ
            printf("ゲームクリア！\n");
        }
        else
        {
            // まだ続くなら、カットイン画面に遷移するだけ
            gs->currentScene = SCENE_STAGE_CLEAR;
            gs->transitionTimer = 2.0f; // カットインを2秒間表示する
        }
    }
}

// プレイヤーの移動関数
static void UpdatePlayer(GameState *gs, const InputState *input)
{
    //  これまでの移動処理
    if (input->up_held)
        gs->player.rect.y -= PLAYER_SPEED;
    if (input->down_held)
        gs->player.rect.y += PLAYER_SPEED;
    if (input->left_held)
        gs->player.rect.x -= PLAYER_SPEED;
    if (input->right_held)
        gs->player.rect.x += PLAYER_SPEED;

    // プレイヤーの移動を制限
    //  画面のサイズを取得
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

    // 上下の壁との判定
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

    switch (gs->difficulty)
    {
    case DIFF_DAY:
        gs->player.hp = 5;
        gs->minigamesRequired = 3;
        gs->veggiesRequired = 2;
        gs->stageTimer = 30.0f;
        // 敵の設定
        gs->enemies[0].isActive = true; // 1体だけ出現
        gs->enemies[0].vx = -5;         // 遅い
        gs->enemies[1].isActive = false;

        gs->arrowCount = 4;
        break;

    case DIFF_EVENING:
        gs->player.hp = 5;
        gs->veggiesRequired = 3;
        gs->minigamesRequired = 4;
        gs->stageTimer = 25.0f;
        // 敵の設定
        gs->enemies[0].isActive = true; // 1体だけ出現
        gs->enemies[0].vx = -7;         // 普通の速さ
        gs->enemies[1].isActive = false;

        gs->arrowCount = 5;
        break;

    case DIFF_NIGHT:
        gs->player.hp = 5;
        gs->veggiesRequired = 4;
        gs->minigamesRequired = 4;
        gs->stageTimer = 25.0f;
        gs->enemies[0].isActive = true;
        gs->enemies[0].vx = -10; // 速い
        gs->enemies[1].isActive = true;
        gs->enemies[1].vx = -8; // 少し速い

        gs->arrowCount = 6;
        break;

    case DIFF_IKUU:
        gs->player.hp = 3;
        gs->veggiesRequired = 5;
        gs->minigamesRequired = 5;
        gs->stageTimer = 15.0f;
        for (int i; i < MAX_ENEMIES; i++)
        {
            if (i < 5)
            { // 最初の5体だけを有効にする
                gs->enemies[i].isActive = true;
                gs->enemies[i].vx = -12 - (rand() % 8); // 速さもランダムに
            }
            else
            {
                gs->enemies[i].isActive = false; // 残りは非表示
            }
        }

        gs->arrowCount = 10;
        break;

    // テスト用
    default:
        /*gs->player.hp = 5;
        gs->veggiesRequired = 2;
        gs->minigamesRequired = 4;
        gs->stageTimer = 30.0f;
        */
        break;
    }

    // （削除予定）gs->minigamesCleared = 0; // クリア数は常に0にリセット
    //   ゲーム開始時・リスタート時にクリア状況をリセット
    if (gs->currentScene == SCENE_TITLE)
    { // タイトルから始める時だけリセット
        gs->minigamesCleared = 0;
        // 難易度に応じて変更する（今は仮に4つとする）
        // gs->minigamesRequired = 4;
    }

    // プレイヤーの状態をリセット
    gs->player.hp = 2;
    // クリアに必要な野菜の数を設定
    gs->veggiesRequired = MAX_VEGGIES;

    // 画面サイズを取得して、プレイヤーを初期位置（左側の中央）に配置
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->player.rect.x = 100;
    gs->player.rect.y = (screen_h - gs->player.rect.h) / 2;

    // 野菜の状態をリセット
    gs->veggiesCollected = 0;
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        gs->veggies[i].isActive = true; // 全ての野菜を有効化
        // 野菜の初期位置を再設定
        gs->veggies[i].rect.x = 600 + (rand() % 800);
        gs->veggies[i].rect.y = 150 + (rand() % 700);
    }

    // ★★★ 敵の初期位置だけを設定する ★★★
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].rect.x = screen_w + (i * 200) + (rand() % 300);
        gs->enemies[i].rect.y = 100 + (rand() % 800);
    }

    // 扉の状態をロック状態で初期化
    gs->door.doorState = DOOR_LOCKED;
    // 扉の初期位置を固定（例：画面右端の中央）
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->door.rect = (SDL_Rect){screen_w - 150, (screen_h - 128) / 2, 128, 128};

    // 扉を閉めたままに
    gs->door.isActive = false;

    // 敵の状態をリセット
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true; // 全ての敵を有効化
        // 敵の初期位置を画面右外に再設定
        gs->enemies[i].rect.x = screen_w + (i * 200) + (rand() % 300);
        gs->enemies[i].rect.y = 100 + (rand() % 800);
    }

    // ミニゲーム2の状態をリセット
    gs->arrowPlayerProgress = 0; // プレイヤーの進捗をリセット
    // 矢印をランダムに再生成
    for (int i = 0; i < MAX_ARROWS; i++)
    {
        gs->arrowSequence[i] = rand() % 4; // 0〜3の乱数を生成
    }
}
// 難易度によって敵の出現数を変動するようにしてるはずが、なぜかたくさん出てきてしまう
//  カットイン用
static void UpdateStageClearScene(GameState *gs)
{
    // transitionTimerを少しずつ減らす
    gs->transitionTimer -= (1.0f / 60.0f);

    // タイマーが0になったら
    if (gs->transitionTimer <= 0)
    {
        StartNewRandomMinigame(gs);          // 次のミニゲームを開始
        gs->currentScene = SCENE_MAIN_STAGE; // 場面をメインステージに戻す
    }
}