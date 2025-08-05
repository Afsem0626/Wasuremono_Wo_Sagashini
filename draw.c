#include "draw.h"
#include <stdio.h>

static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);
static void DrawTitleScene(GameState *gs);
static void DrawMainStage(GameState *gs);
static void DrawNovelScene(GameState *gs);
static void DrawStageClearScene(GameState *gs);
static void DrawGameOverScene(GameState *gs);
static void DrawPreEndingCutscene(GameState *gs);
static void DrawEndingScene(GameState *gs);
static void DrawThanksScene(GameState *gs);
static void DrawVeggieMinigame(GameState *gs);
static void DrawArrowMinigame(GameState *gs);
static void DrawHUD(GameState *gs);

void DrawGame(GameState *gs)
{
    // 背景をクリア
    SDL_SetRenderDrawColor(gs->renderer, 200, 200, 220, 255); // 明るい灰色
    SDL_RenderClear(gs->renderer);

    // 現在のシーンに応じて描画を切り替え
    switch (gs->currentScene)
    {
    case SCENE_TITLE:
        DrawTitleScene(gs);
        break;
    case SCENE_DIFFICULTY:
        DrawDifficultyScene(gs);
        break;
    case SCENE_MAIN_STAGE:
        DrawMainStage(gs);
        break;
    case SCENE_STAGE_CLEAR: // カットイン用
        DrawStageClearScene(gs);
        break;
    case SCENE_GAME_OVER:
        DrawGameOverScene(gs);
        break;
    case SCENE_NOVEL:
        DrawNovelScene(gs);
        break;
    case SCENE_PRE_ENDING_CUTSCENE:
        DrawPreEndingCutscene(gs);
        break;
    case SCENE_ENDING:
        DrawEndingScene(gs);
        break;
    case SCENE_THANKS:
        DrawThanksScene(gs);
        break;
    }
    SDL_RenderPresent(gs->renderer);
}

void DrawDifficultyScene(GameState *gs)
{
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // 難易度の選択肢
    const char *difficulty_names[] = {
        "昼 (Easy)",
        "夕方 (Normal)",
        "深夜 (Hard)",
        "異空 (Expert)"};

    // 画面をクリア（背景色）
    // 修正箇所
    SDL_SetRenderDrawColor(gs->renderer, 30, 30, 50, 255); // 深い青色
    SDL_RenderClear(gs->renderer);

    // ヘッダーテキストを描画
    DrawText(gs->renderer, gs->font, "難易度を選んでください", 650, 200, white);

    // 各選択肢をループで描画
    for (int i = 0; i < DIFFICULTY_COUNT; i++)
    {
        // 現在選択されている項目かどうかを判定
        if (i == gs->difficultySelection)
        {
            DrawText(gs->renderer, gs->font, "→", 700, 400 + i * 100, yellow);
            DrawText(gs->renderer, gs->font, difficulty_names[i], 750, 400 + i * 100, white);
        }
        else
        {

            DrawText(gs->renderer, gs->font, difficulty_names[i], 750, 400 + i * 100, white);
        }
    }
}

static void DrawTitleScene(GameState *gs)
{
    SDL_Color white = {255, 255, 255, 255};
    if (gs->titleTexture != NULL)
    {
        SDL_RenderCopy(gs->renderer, gs->titleTexture, NULL, NULL);
    }
    else
    {
        DrawText(gs->renderer, gs->font, "タイトル画像を読み込めませんでした", 500, 500, white);
    }
}

static void DrawGameOverScene(GameState *gs)
{
    // 背景を念のため黒でクリア
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);

    if (gs->gameOverTexture != NULL)
    {
        SDL_RenderCopy(gs->renderer, gs->gameOverTexture, NULL, NULL);
    }
    else
    {
        // もし画像の読み込みに失敗していたら、代わりにテキストを表示
        SDL_Color red = {255, 0, 0, 255};
        DrawText(gs->renderer, gs->font, "GAME OVER", 760, 490, red);
    }
}

static void DrawMainStage(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 135, 206, 235, 255);
    SDL_RenderClear(gs->renderer);
    // 現在のミニゲームに応じて描画内容を切り替える
    if (gs->currentMinigame == MINIGAME_VEGGIE)
    {
        DrawVeggieMinigame(gs);
    }
    else if (gs->currentMinigame == MINIGAME_ARROWS)
    {
        DrawArrowMinigame(gs);
    }

    // メッセージボックスの描画 (画面下部に配置)

    SDL_Rect msgBoxRect = {100, 880, 1720, 150}; //

    // 半透明の描画を有効にする
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    // 描画色を半透明の黒に設定
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 180); // メモ 最後の引数は透明度

    // 四角形を塗りつぶして描画
    SDL_RenderFillRect(gs->renderer, &msgBoxRect);

    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_NONE);

    // キャラクターアイコンの描画
    SDL_Rect iconRect = {120, 830, 200, 200};

    SDL_RenderCopy(gs->renderer, gs->iconTexture, NULL, &iconRect);

    // ゲーム内メッセージの描画
    if (gs->gameMessage[0] != '\0')
    {
        SDL_Color white = {255, 255, 255, 255};

        DrawText(gs->renderer, gs->font, gs->gameMessage, 300, 930, white);
    }
    DrawHUD(gs);
}

static void DrawNovelScene(GameState *gs)
{
    // 背景の描画
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);

    // キャラクターの立ち絵を描画 (かなり右側に配置)
    SDL_Rect charRect = {1200, 100, 600, 900};
    SDL_RenderCopy(gs->renderer, gs->openingNovel.characterTexture, NULL, &charRect);

    SDL_Rect msgBoxRect = {100, 780, 1720, 250};

    // 半透明の描画を有効にする
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    // 描画色を半透明の黒に設定
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 180); // 最後の180が透明度

    SDL_RenderFillRect(gs->renderer, &msgBoxRect);

    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_NONE);

    // 現在の行のテキストを描画
    if (gs->openingNovel.currentLine < gs->openingNovel.lineCount)
    {
        SDL_Color white = {255, 255, 255, 255};
        DrawText(gs->renderer, gs->font, gs->openingNovel.lines[gs->openingNovel.currentLine], 150, 830, white);
    }
}

static void DrawStageClearScene(GameState *gs)
{
    // まず、背景としてメインステージの画面をそのまま描画する
    DrawMainStage(gs);

    // その上に、半透明の黒いフィルターをかける
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(gs->renderer, NULL); // 画面全体を覆う

    // ステージクリアの文字を中央に描画
    SDL_Color white = {255, 255, 255, 255};
    DrawText(gs->renderer, gs->largeFont, "STAGE CLEAR!", 700, 450, white);
}

static void DrawEndingScene(GameState *gs)
{

    SDL_SetRenderDrawColor(gs->renderer, 250, 250, 210, 255);
    SDL_RenderClear(gs->renderer);

    // 立ち絵の描画
    SDL_Rect charRect = {1200, 100, 600, 900};
    SDL_RenderCopy(gs->renderer, gs->endingCharTexture, NULL, &charRect);

    // メッセージボックスの描画
    SDL_Rect msgBoxRect = {100, 780, 1720, 250}; // 位置とサイズは要調整

    // 半透明の描画を有効にし、色を設定
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 180); // 半透明の黒

    // 四角形を塗りつぶして描画
    SDL_RenderFillRect(gs->renderer, &msgBoxRect);

    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_NONE);

    // テキストの描画
    if (gs->endingNovel.currentLine < gs->endingNovel.lineCount)
    {
        SDL_Color white = {255, 255, 255, 255};
        DrawText(gs->renderer, gs->font, gs->endingNovel.lines[gs->endingNovel.currentLine], 150, 830, white);
    }
}

static void DrawThanksScene(GameState *gs)
{
    SDL_SetRenderDrawColor(gs->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gs->renderer);

    if (gs->thanksTexture != NULL)
    {

        SDL_RenderCopy(gs->renderer, gs->thanksTexture, NULL, NULL);
    }
    else
    {
        // もし画像の読み込みに失敗していたら、代わりにテキストを表示
        SDL_Color white = {255, 255, 255, 255};
        DrawText(gs->renderer, gs->font, "Thank You For Playing!", 600, 500, white);
    }
}

static void DrawVeggieMinigame(GameState *gs)
{
    // 野菜の描画
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].isActive)
        {
            SDL_RenderCopy(gs->renderer, gs->veggies[i].texture, NULL, &gs->veggies[i].rect);
        }
    }
    // 敵の描画
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (gs->enemies[i].isActive)
        {
            SDL_RenderCopy(gs->renderer, gs->enemies[i].texture, NULL, &gs->enemies[i].rect);
        }
    }
    // 扉の描画
    if (gs->door.doorState == DOOR_LOCKED)
    {
        SDL_RenderCopy(gs->renderer, gs->doorLockedTexture, NULL, &gs->door.rect);
    }
    else if (gs->door.doorState == DOOR_UNLOCKED)
    {
        SDL_RenderCopy(gs->renderer, gs->doorUnlockedTexture, NULL, &gs->door.rect);
    }
    // プレイヤーとHUDを描画
    SDL_RenderCopy(gs->renderer, gs->player.texture, NULL, &gs->player.rect);
    DrawHUD(gs);
}

static void DrawArrowMinigame(GameState *gs)
{
    int size = 256;    // 矢印のサイズ
    int spacing = 300; // 矢印同士の間隔
    int y = 400;
    int thickness = 10; // 線の太さ
    int first_arrow_x = 400;

    // アニメーションの進行度を計算する
    float progress = 0.0f;
    if (gs->isArrowAnimating)
    {
        progress = gs->arrowAnimationTimer / ARROW_ANIMATION_DURATION;
        if (progress > 1.0f)
            progress = 1.0f;
    }

    // 画面に表示する矢印の描画
    for (int i = 0; i < gs->arrowCount - gs->arrowPlayerProgress; i++)
    {
        // 画面外に出る矢印は描画しない
        int current_x = first_arrow_x + i * spacing;
        if (current_x > 1920)
        {
            break;
        }

        SDL_Texture *tex = gs->arrowTextures[(i < MAX_ARROWS) ? gs->arrowSequence[(gs->arrowPlayerProgress + i) % MAX_ARROWS] : 0];

        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);

        SDL_Rect destRect = {current_x, y, size, size};

        // アニメーション中の描画処理
        if (gs->isArrowAnimating)
        {
            // 今まさに消えつつある矢印 (i=0は先頭の矢印)
            if (i == 0)
            {
                // だんだん透明にする (アルファ値を変更)
                SDL_SetTextureAlphaMod(tex, 255 * (1.0f - progress));
            }
            // 左にスライドしてくる後続の矢印
            else
            {
                destRect.x -= spacing * progress; // progressに応じて左にずらす
            }
        }
        SDL_RenderCopy(gs->renderer, tex, NULL, &destRect);
    }

    // 描画色を不透明度0の白に設定
    SDL_SetRenderDrawColor(gs->renderer, 255, 255, 255, 255);

    // 4つの塗りつぶされた四角形で「枠」を描く
    SDL_Rect top_bar = {first_arrow_x, y, size, thickness};
    SDL_RenderFillRect(gs->renderer, &top_bar);

    SDL_Rect bottom_bar = {first_arrow_x, y + size - thickness, size, thickness};
    SDL_RenderFillRect(gs->renderer, &bottom_bar);

    SDL_Rect left_bar = {first_arrow_x, y, thickness, size};
    SDL_RenderFillRect(gs->renderer, &left_bar);

    SDL_Rect right_bar = {first_arrow_x + size - thickness, y, thickness, size};
    SDL_RenderFillRect(gs->renderer, &right_bar);

    DrawHUD(gs);
}

static void DrawHUD(GameState *gs)
{
    char hudText[128];

    sprintf(hudText, "ステージ: %d / %d | HP: %d | 野菜: %d / %d | 時間: %.0f",
            gs->minigamesCleared + 1,
            gs->minigamesRequired,
            gs->player.hp,
            gs->veggiesCollected,
            gs->veggiesRequired,
            gs->stageTimer);

    SDL_Color white = {255, 255, 255, 255};
    // SDL_Color black = {0, 0, 0, 255};

    DrawText(gs->renderer, gs->font, hudText, 20, 20, white);
    if (gs->currentMinigame == MINIGAME_VEGGIE)
    {
        DrawText(gs->renderer, gs->font, "おつかいリスト:", 1200, 20, (SDL_Color){255, 255, 255, 255});
        for (int i = 0; i < gs->targetVeggieCount; i++)
        {
            SDL_Rect targetRect = {1500 + i * 90, 10, 80, 80}; // 90ピクセルずつ右にずらす
            SDL_RenderCopy(gs->renderer, gs->veggieTextures[gs->targetVeggieTypes[i]], NULL, &targetRect);
        }
    }
}

static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    if (!surface)
        return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (!texture)
        return;
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}

// draw.c に追加
static void DrawPreEndingCutscene(GameState *gs)
{
    // まず、背景としてメインステージの画面をそのまま描画する
    DrawMainStage(gs);

    // その上に、半透明の白いフィルターをかける
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gs->renderer, 255, 255, 255, 128); // 半透明の白
    SDL_RenderFillRect(gs->renderer, NULL);

    // ゲームクリア文字を中央に描画
    SDL_Color black = {0, 0, 0, 255};
    DrawText(gs->renderer, gs->largeFont, "ALL STAGES CLEAR!", 550, 350, black);
    DrawText(gs->renderer, gs->largeFont, "Congratulations!", 550, 500, black);
}