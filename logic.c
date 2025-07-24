#include "logic.h"
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const int PLAYER_SPEED = 10;

static void UpdateTitleScene(GameState *gs, const InputState *input);
static void UpdateDifficultyScene(GameState *gs, const InputState *input);
static void UpdateNovelScene(GameState *gs, const InputState *input);
static void SetGameMessage(GameState *gs, const char *message, float duration);
static void UpdateGameOverScene(GameState *gs, const InputState *input);
static void UpdatePreEndingCutscene(GameState *gs);
static void UpdateEndingScene(GameState *gs, const InputState *input);
static void UpdateThanksScene(GameState *gs, const InputState *input);
static void UpdateVeggieMinigame(GameState *gs, const InputState *input);
static void UpdateStageClearScene(GameState *gs);
static void UpdateArrowMinigame(GameState *gs, const InputState *input);
static void StartNewRandomMinigame(GameState *gs);
static void ResetStage(GameState *gs);
static void UpdatePlayer(GameState *gs, const InputState *input);
static void UpdateEnemies(GameState *gs);
static void CheckCollisions(GameState *gs, const InputState *input);
static bool DetectCollision(const SDL_Rect *a, const SDL_Rect *b);

void UpdateGame(GameState *gs, const InputState *input)
{
    if (gs->messageTimer > 0)
    {
        gs->messageTimer -= (1.0f / 60.0f);
        if (gs->messageTimer <= 0)
        {
            gs->gameMessage[0] = '\0'; // タイマーが切れたらメッセージを空にする
            // メッセージが空になったら、次の行へ進む
        }
    }

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
            PlaySound(gs->gameOverSound);
            gs->currentScene = SCENE_GAME_OVER;
            gs->transitionTimer = 1.5f;
            // printf("時間切れ！ゲームオーバー！\n");
            break;
        }
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
        UpdateNovelScene(gs, input);
        break;
    case SCENE_PRE_ENDING_CUTSCENE:
        UpdatePreEndingCutscene(gs);
        break;
    case SCENE_ENDING:
        UpdateEndingScene(gs, input);
        break;
    case SCENE_THANKS:
        UpdateThanksScene(gs, input);
        break;
    }
}

static void UpdateTitleScene(GameState *gs, const InputState *input)
{
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed)
    {
        // StartNewRandomMinigame(gs);
        // gs->currentScene = SCENE_DIFFICULTY;
        gs->openingNovel.currentLine = 0; // 会話を最初からにする
        gs->currentScene = SCENE_NOVEL;
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

    if (input->a_pressed)
    {
        // 選択されているカーソル位置を、実際の難易度として設定
        gs->difficulty = (Difficulty)gs->difficultySelection;

        gs->minigamesCleared = 0;
        // ステージをリセットしてゲームを開始
        StartNewRandomMinigame(gs);
        gs->currentScene = SCENE_MAIN_STAGE;
        // printf("難易度 %d でゲーム開始！\n", gs->difficulty);
    }
}

static void UpdateNovelScene(GameState *gs, const InputState *input)
{
    // 何かパネルが「押された瞬間」なら
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed || input->a_pressed || input->b_pressed)
    {
        gs->openingNovel.currentLine++; // 次の行へ

        // もし、スクリプトの最後まで進んだら
        if (gs->openingNovel.currentLine >= gs->openingNovel.lineCount)
        {
            // 次のシーン（難易度選択）へ移行
            gs->currentScene = SCENE_DIFFICULTY;
        }
    }
}

// ゲーム内メッセージを設定する関数
static void SetGameMessage(GameState *gs, const char *message, float duration)
{
    strncpy(gs->gameMessage, message, sizeof(gs->gameMessage) - 1);
    gs->gameMessage[sizeof(gs->gameMessage) - 1] = '\0'; // 安全対策
    gs->messageTimer = duration;                         // メッセージを表示する時間（秒）
}

static void UpdateEndingScene(GameState *gs, const InputState *input)
{
    if (gs->transitionTimer > 0)
    {
        gs->transitionTimer -= (1.0f / 60.0f);
        return; // タイマー作動中はキー入力を受け付けない
    }

    // タイマー終了後、何かパネルが「押された瞬間」なら
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed || input->a_pressed || input->b_pressed)
    {
        // 次の行へ
        gs->endingNovel.currentLine++;

        if (gs->endingNovel.currentLine >= gs->endingNovel.lineCount)
        {
            // 次に表示するオープニング会話の行を0に戻しておく
            gs->openingNovel.currentLine = 0;
            gs->currentScene = SCENE_THANKS;
        }
    }
}

static void UpdateThanksScene(GameState *gs, const InputState *input)
{
    // 何かパネルが「押された瞬間」なら、タイトル画面に戻る
    if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed || input->a_pressed || input->b_pressed)
    {
        gs->currentScene = SCENE_TITLE;
    }
}

static void UpdateGameOverScene(GameState *gs, const InputState *input)
{
    // タイマーがまだ残っている間は、それを減らすだけ
    if (gs->transitionTimer > 0)
    {
        gs->transitionTimer -= (1.0f / 60.0f); // 60FPSを仮定
    }
    // タイマーが終わってから、初めてキー入力を受け付ける
    else
    {
        // 何かパネルが「押された瞬間」なら、タイトル画面に戻る
        if (input->up_pressed || input->down_pressed || input->left_pressed || input->right_pressed || input->a_pressed || input->b_pressed)
        {
            gs->currentScene = SCENE_TITLE;
        }
    }
}

static void UpdateVeggieMinigame(GameState *gs, const InputState *input)
{
    UpdatePlayer(gs, input);
    UpdateEnemies(gs);
    CheckCollisions(gs, input);
    if (gs->player.hp <= 0)
    {
        gs->currentScene = SCENE_GAME_OVER;
        gs->transitionTimer = 1.5f;
    }
    if (gs->veggiesCollected >= gs->veggiesRequired && gs->door.doorState == DOOR_LOCKED)
    {
        // printf("全ての野菜を回収！ 扉のロックが解除された！\n");
        gs->door.doorState = DOOR_UNLOCKED;
        SetGameMessage(gs, "見て！先に進めるみたい！", 5.0f);
        // ここでロック解除の効果音を鳴らす予定
    }
}

static void UpdateArrowMinigame(GameState *gs, const InputState *input)
{
    if (gs->isArrowAnimating)
    {
        // タイマーを進める (1/60.0fは60FPSを仮定)
        gs->arrowAnimationTimer += (1.0f / 60.0f);

        // アニメーションが完了したら
        if (gs->arrowAnimationTimer >= ARROW_ANIMATION_DURATION)
        {
            gs->isArrowAnimating = false; // アニメーション終了
            gs->arrowPlayerProgress++;

            if (gs->arrowPlayerProgress >= gs->arrowCount)
            {
                // printf("ミニゲーム2 クリア！\n");
                gs->minigamesCleared++;

                if (gs->minigamesCleared >= gs->minigamesRequired)
                {
                    gs->currentScene = SCENE_PRE_ENDING_CUTSCENE;
                    gs->transitionTimer = 1.5f; // 特別なので少し長めに3秒表示
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

    // 入力待ちの処理
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
        // PlaySound(gs->damageSound);
        SetGameMessage(gs, "ミスった～！最初からになっちゃった！", 2.0f); // 2秒間メッセージを表示
        gs->arrowPlayerProgress = 0;                                      // 最初からやり直し
    }

    // HPが0になったらゲームオーバー
    if (gs->player.hp <= 0)
    {
        PlaySound(gs->gameOverSound);
        gs->currentScene = SCENE_GAME_OVER;
        gs->transitionTimer = 1.5f;
    }
}

static void StartNewRandomMinigame(GameState *gs)
{
    ResetStage(gs);
    if (rand() % 3 != 2)
    {
        gs->currentMinigame = MINIGAME_VEGGIE;
        // printf("次のミニゲーム: 野菜集め\n");
        SetGameMessage(gs, "まずは野菜を拾わなくちゃ！(マットのAボタン、それかZキーで拾えるよ～)", 5.0f); // 5秒間表示
    }
    else
    {
        gs->currentMinigame = MINIGAME_ARROWS;
        // printf("次のミニゲーム: 矢印入力\n");
        SetGameMessage(gs, "同じ矢印を踏んで迷わず帰ろう！", 5.0f); // 5秒間表示
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

static void CheckCollisions(GameState *gs, const InputState *input)
{
    // logic.c の CheckCollisions 関数（野菜との当たり判定部分）

    // 野菜との当たり判定
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive && DetectCollision(&gs->player.rect, &gs->veggies[i].rect) &&
            input->a_pressed)
        {
            bool isTarget = false;
            for (int j = 0; j < gs->targetVeggieCount; j++)
            {
                if (gs->veggies[i].veggieType == gs->targetVeggieTypes[j])
                {
                    isTarget = true;
                    break;
                }
            }

            if (isTarget)
            {
                gs->veggies[i].isActive = false;
                gs->veggiesCollected++;
                PlaySound(gs->veggieGetSound);
            }
            else
            {
                // 当初はhpを減らすようにしていたが、鬼畜すぎたので修正
                /*
                gs->player.hp--;
                PlaySound(gs->damageSound);
                gs->veggies[i].isActive = false;
                */
            }
        }
    }

    // 敵との当たり判定
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive)
        {
            SDL_Rect playerHitbox = gs->player.rect;
            playerHitbox.x += 10; // 左右から20ピクセルずつ内側に
            playerHitbox.w -= 20; // その分、幅を合計40ピクセル小さくする
            playerHitbox.y += 10; // 上下から20ピクセルずつ内側に
            playerHitbox.h -= 20; // その分、高さを合計40ピクセル小さくする

            SDL_Rect enemyHitbox = gs->enemies[i].rect;
            enemyHitbox.x += 15; // 左右から15ピクセルずつ内側に
            enemyHitbox.w -= 30;
            enemyHitbox.y += 15; // 上下から15ピクセルずつ内側に
            enemyHitbox.h -= 30;

            if (DetectCollision(&playerHitbox, &enemyHitbox))
            {
                gs->player.hp--;
                gs->enemies[i].isActive = false;
                // PlaySound(gs->damageSound);
                SetGameMessage(gs, "痛ったー！ぶつかっちゃった...", 2.0f);
            }
        }
    }
    // 扉との当たり判定
    if (gs->player.hp > 0 && gs->door.doorState == DOOR_UNLOCKED && DetectCollision(&gs->player.rect, &gs->door.rect))
    {
        // printf("扉に入った！ミニゲーム1 クリア！カットインへ\n");
        gs->minigamesCleared++;
        if (gs->minigamesCleared >= gs->minigamesRequired)
        {
            gs->endingNovel.currentLine = 0;
            gs->currentScene = SCENE_PRE_ENDING_CUTSCENE; // カットインへ
            gs->transitionTimer = 3.0f;                   // 特別なので少し長
        }
        else
        {
            gs->currentScene = SCENE_STAGE_CLEAR;
            gs->transitionTimer = 2.0f;
        }
    }
}

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
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);

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
    // printf("ステージをリセットします。\n");

    switch (gs->difficulty)
    {
    case DIFF_DAY:
        gs->player.hp = 5;
        gs->targetVeggieCount = 1;
        gs->veggiesRequired = 1;
        gs->minigamesRequired = 1;
        gs->stageTimer = 30.0f;
        // 敵の設定
        gs->enemies[0].isActive = true;
        gs->enemies[0].vx = -5;
        gs->enemies[1].isActive = false;

        gs->arrowCount = 4;
        break;

    case DIFF_EVENING:
        gs->player.hp = 5;
        gs->targetVeggieCount = 2;
        gs->veggiesRequired = 2;
        gs->minigamesRequired = 4;
        gs->stageTimer = 25.0f;
        // 敵の設定
        gs->enemies[0].isActive = true;
        gs->enemies[0].vx = -7;
        gs->enemies[1].isActive = false;

        gs->arrowCount = 5;
        break;

    case DIFF_NIGHT:
        gs->player.hp = 5;
        gs->targetVeggieCount = 3;
        gs->veggiesRequired = 3;
        gs->minigamesRequired = 4;
        gs->stageTimer = 25.0f;
        gs->enemies[0].isActive = true;
        gs->enemies[0].vx = -10;
        gs->enemies[1].isActive = true;
        gs->enemies[1].vx = -8;

        gs->arrowCount = 6;
        break;

    case DIFF_IKUU:
        gs->player.hp = 3;
        gs->targetVeggieCount = 5;
        gs->veggiesRequired = 5;
        gs->minigamesRequired = 5;
        gs->stageTimer = 15.0f;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (i < 5)
            {
                gs->enemies[i].isActive = true;
                gs->enemies[i].vx = -12 - (rand() % 8); // 速さもランダムに
            }
            else
            {
                gs->enemies[i].isActive = false;
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

    if (gs->currentScene == SCENE_TITLE)
    { // タイトルから始める時だけリセット
        gs->minigamesCleared = 0;
    }

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);
    gs->player.rect.x = 100;
    gs->player.rect.y = (screen_h - gs->player.rect.h) / 2;

    // 野菜の状態をリセット

    VeggieType all_veggies[VEGGIE_TYPE_COUNT] = {VEGGIE_CARROT, VEGGIE_EGGPLANT, VEGGIE_TOMATO, VEGGIE_TURNIP, VEGGIE_MUSHROOM};
    // 配列をシャッフルする
    for (int i = 0; i < VEGGIE_TYPE_COUNT; i++)
    {
        int j = rand() % VEGGIE_TYPE_COUNT;
        VeggieType temp = all_veggies[i];
        all_veggies[i] = all_veggies[j];
        all_veggies[j] = temp;
    }
    // シャッフルした配列の先頭から、必要な数だけをリストとしてコピー
    for (int i = 0; i < gs->targetVeggieCount; i++)
    {
        gs->targetVeggieTypes[i] = all_veggies[i];
    }

    // 野菜の状態をリセット
    gs->veggiesCollected = 0;
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (i < MAX_VEGGIES)
        {
            gs->veggies[i].isActive = true; // 表示する
            gs->veggies[i].rect.x = 300 + (rand() % (screen_w - 400));
            gs->veggies[i].rect.y = 100 + (rand() % (screen_h - 200));

            VeggieType type = (VeggieType)i;
            gs->veggies[i].veggieType = type;
            gs->veggies[i].texture = gs->veggieTextures[type];
        }
        else
        {
            gs->veggies[i].isActive = false; // それ以外は非表示
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].rect.x = screen_w + (i * 200) + (rand() % 300);
        // gs->enemies[i].rect.y = 100 + (rand() % 800);
        gs->enemies[i].rect.y = rand() % (screen_h - gs->enemies[i].rect.h);
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
        gs->enemies[i].rect.y = 20 + (rand() % (screen_h - 150));
    }

    gs->arrowPlayerProgress = 0; // プレイヤーの進度をリセット
    // 矢印をランダムに再生成
    for (int i = 0; i < MAX_ARROWS; i++)
    {
        gs->arrowSequence[i] = rand() % 4; // 0〜3の乱数を生成
    }
}
//  カットイン用
static void UpdateStageClearScene(GameState *gs)
{
    // transitionTimerを少しずつ減らす
    gs->transitionTimer -= (1.0f / 60.0f);

    if (gs->transitionTimer <= 0)
    {
        StartNewRandomMinigame(gs);          // 次のミニゲームを開始
        gs->currentScene = SCENE_MAIN_STAGE; // 場面をメインステージに戻す
    }
}

static void UpdatePreEndingCutscene(GameState *gs)
{
    // transitionTimerを少しずつ減らす
    gs->transitionTimer -= (1.0f / 60.0f);

    // タイマーが0になったら
    if (gs->transitionTimer <= 0)
    {
        gs->endingNovel.currentLine = 0; // エンディング会話を最初からに
        gs->currentScene = SCENE_ENDING; // 場面をエンディングに切り替える
        gs->transitionTimer = 1.5f;      // フェードイン用にタイマーを再設定
    }
}