#include "init.h"
#include <stdio.h>
#include <stdlib.h> // ★★★ srand(), rand() のために追加 ★★★
#include <time.h>   // ★★★ time() のために追加 ★★★

static void LoadScript(const char *path, NovelState *novel);

// テクスチャ読み込みの補助関数
SDL_Texture *LoadTexture(const char *path, SDL_Renderer *renderer)
{
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (!loadedSurface)
    {
        fprintf(stderr, "画像をロードできませんでした: %s! SDL_image Error: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!newTexture)
    {
        fprintf(stderr, "テクスチャを作成できませんでした: %s! SDL Error: %s\n", path, SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

// アセットを読み込む専用の関数
void LoadAssets(GameState *gs)
{
    // フォント
    gs->font = TTF_OpenFont("ZenOldMincho-Bold.ttf", 28);
    if (!gs->font)
    { /* エラー処理 */
        return;
    }
    gs->largeFont = TTF_OpenFont("ZenOldMincho-Bold.ttf", 72); // 例：大きめのフォントサイズ
    if (!gs->largeFont)
    {
        fprintf(stderr, "大きめのフォントのロードに失敗: %s\n", TTF_GetError());
        return;
    }

    // タイトル画面
    gs->titleTexture = LoadTexture("assets/title_screen.png", gs->renderer);

    // プレイヤー画像（現在は青い箱）
    gs->player.texture = LoadTexture("assets/player.png", gs->renderer);

    // ノベル用
    gs->openingNovel.characterTexture = LoadTexture("assets/yuri_stand.png", gs->renderer);
    gs->endingCharTexture = LoadTexture("assets/yuri_ending.png", gs->renderer);
    gs->iconTexture = LoadTexture("assets/yuri_icon.png", gs->renderer);
    gs->thanksTexture = LoadTexture("assets/thanks.png", gs->renderer);
    gs->gameOverTexture = LoadTexture("assets/game_over.png", gs->renderer);

    gs->openingNovel.windowTexture = LoadTexture("assets/message_window.png", gs->renderer);

    // テキストファイルを読み込む (この補助関数は後で作成)
    LoadScript("assets/opening.txt", &gs->openingNovel);
    LoadScript("assets/ending.txt", &gs->endingNovel);

    // 野菜画像
    /*gs->veggies[0].texture = LoadTexture("vegetables/carrot.png", gs->renderer);
    gs->veggies[1].texture = LoadTexture("vegetables/eggplant.png", gs->renderer);
    gs->veggies[2].texture = LoadTexture("vegetables/tomato.png", gs->renderer);
    gs->veggies[3].texture = LoadTexture("vegetables/turnip.png", gs->renderer);   // ★★★ 追加 ★★★
    gs->veggies[4].texture = LoadTexture("vegetables/mushroom.png", gs->renderer); // ★★★ 追加 ★★★
    */
    gs->veggieTextures[VEGGIE_CARROT] = LoadTexture("vegetables/carrot.png", gs->renderer);
    gs->veggieTextures[VEGGIE_EGGPLANT] = LoadTexture("vegetables/eggplant.png", gs->renderer);
    gs->veggieTextures[VEGGIE_TOMATO] = LoadTexture("vegetables/tomato.png", gs->renderer);
    gs->veggieTextures[VEGGIE_TURNIP] = LoadTexture("vegetables/turnip.png", gs->renderer);
    gs->veggieTextures[VEGGIE_MUSHROOM] = LoadTexture("vegetables/mushroom.png", gs->renderer);

    // 扉の画像
    gs->doorLockedTexture = LoadTexture("assets/door_locked.png", gs->renderer);
    gs->doorUnlockedTexture = LoadTexture("assets/door_unlocked.png", gs->renderer);

    // まず、敵のテクスチャを一度だけ読み込む
    SDL_Texture *enemyTexture = LoadTexture("assets/enemy.png", gs->renderer);

    SDL_Surface *surface = IMG_Load("assets/thanks.png");
    if (!surface)
    {
        fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
        // エラー処理（ゲームを終了するなど）を検討してください
        return;
    }
    gs->thanksTexture = SDL_CreateTextureFromSurface(gs->renderer, surface);
    SDL_FreeSurface(surface);
    if (!gs->thanksTexture)
    {
        fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        // エラー処理（ゲームを終了するなど）を検討してください
        return;
    }

    // forループを使って、全ての敵に同じテクスチャを設定する
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].texture = enemyTexture;
    }

    // 効果音
    gs->damageSound = Mix_LoadWAV("sound/damage.wav");
    if (!gs->damageSound)
    { /* エラー処理 */
    }
    gs->veggieGetSound = Mix_LoadWAV("sound/get.wav");
    gs->gameOverSound = Mix_LoadWAV("sound/gameover.wav");

    // 矢印テクスチャの読み込み
    gs->arrowTextures[ARROW_UP] = LoadTexture("assets/arrow_up.png", gs->renderer);
    gs->arrowTextures[ARROW_DOWN] = LoadTexture("assets/arrow_down.png", gs->renderer);
    gs->arrowTextures[ARROW_LEFT] = LoadTexture("assets/arrow_left.png", gs->renderer);
    gs->arrowTextures[ARROW_RIGHT] = LoadTexture("assets/arrow_right.png", gs->renderer);
}

static void LoadScript(const char *path, NovelState *novel)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "スクリプトファイルを開けませんでした: %s\n", path);
        return;
    }

    char buffer[256];
    int count = 0;
    // まず行数を数える
    while (fgets(buffer, sizeof(buffer), file))
    {
        count++;
    }

    novel->lineCount = count;
    novel->lines = malloc(sizeof(char *) * count);
    if (novel->lines == NULL)
    { /* エラー処理 */
        fclose(file);
        return;
    }

    // ファイルポインタを先頭に戻す
    fseek(file, 0, SEEK_SET);

    // 各行を読み込んでメモリに格納
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        // 改行文字を削除
        buffer[strcspn(buffer, "\n")] = 0;
        novel->lines[i] = strdup(buffer);
        if (novel->lines[i] == NULL)
        { /* エラー処理 */
            break;
        }
        i++;
    }

    fclose(file);
}

bool InitGame(GameState *gs)
{
    srand(time(NULL));
    // ライブラリの初期化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ||
        !(IMG_Init(IMG_INIT_PNG)) ||
        TTF_Init() == -1 ||
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "ライブラリの初期化に失敗しました: %s\n", SDL_GetError());
        return false;
    }

    // 修正前
    // gs->window = SDL_CreateWindow("Wasuremono Wo Sagashini",
    //                               SDL_WINDOWPOS_UNDEFINED,
    //                               SDL_WINDOWPOS_UNDEFINED,
    //                               1920,
    //                               1080,
    //                               SDL_WINDOW_SHOWN);

    // 修正後
    gs->window = SDL_CreateWindow("忘れ物を探シニ",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  0, // 0に設定
                                  0, // 0に設定
                                  SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!gs->window)
    { /* エラー処理 */
        return false;
    }
    gs->renderer = SDL_CreateRenderer(gs->window, -1, SDL_RENDERER_SOFTWARE);
    if (!gs->window || !gs->renderer)
        return false;
    // レンダラーで半透明の描画を有効にする
    SDL_SetRenderDrawBlendMode(gs->renderer, SDL_BLENDMODE_BLEND);

    // ジョイスティックの準備

    if (SDL_NumJoysticks() > 0)
        gs->ddrMat = SDL_JoystickOpen(0);

    // アセット読み込み
    LoadAssets(gs);

    // --- 各オブジェクトの初期状態を設定 ---
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(gs->renderer, &screen_w, &screen_h);

    // プレイヤー
    gs->player.hp = 5;
    gs->player.rect = (SDL_Rect){100, (screen_h - 100) / 2, 100, 100};

    // 野菜
    /**/
    gs->veggiesCollected = 0;
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        gs->veggies[i].isActive = false;
        gs->veggies[i].rect = (SDL_Rect){0, 0, 160, 160}; // 初期位置は後で設定
        gs->veggies[i].texture = NULL;                    // テクスチャは後で読み込む
    }
    // 敵
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        gs->enemies[i].isActive = true;
        gs->enemies[i].rect = (SDL_Rect){screen_w + (i * 400), 200 + i * 300, 120, 120};
        gs->enemies[i].vx = -5;
        gs->enemies[i].vy = 0;
    }

    gs->isRunning = true;

    // お題となる矢印を初期化
    gs->arrowSequence[0] = ARROW_LEFT;
    gs->arrowSequence[1] = ARROW_RIGHT;
    gs->arrowSequence[2] = ARROW_DOWN;
    gs->arrowSequence[3] = ARROW_UP;
    // 初期シーンをタイトル画面に設定
    gs->currentScene = SCENE_TITLE;
    gs->difficultySelection = 0; // 難易度選択用カーソル
    // gs->currentScene = SCENE_MAIN_STAGE;
    return true;
}

void Cleanup(GameState *gs)
{
    // テクスチャの解放
    SDL_DestroyTexture(gs->player.texture);
    for (int i = 0; i < MAX_VEGGIES; i++)
    {
        if (gs->veggies[i].texture)
            SDL_DestroyTexture(gs->veggies[i].texture);
    }
    // 敵のテクスチャは共有しているので一度だけ解放
    if (gs->enemies[0].texture)
        SDL_DestroyTexture(gs->enemies[0].texture);

    // 会話シーン用テクスチャの解放
    SDL_DestroyTexture(gs->openingNovel.characterTexture);
    SDL_DestroyTexture(gs->openingNovel.windowTexture);

    SDL_DestroyTexture(gs->endingNovel.characterTexture);
    SDL_DestroyTexture(gs->endingNovel.windowTexture);

    SDL_DestroyTexture(gs->gameOverTexture);

    SDL_DestroyTexture(gs->iconTexture);
    SDL_DestroyTexture(gs->endingCharTexture);

    if (gs->thanksTexture)
    {
        SDL_DestroyTexture(gs->thanksTexture);
    }

    // スクリプト用メモリの解放
    for (int i = 0; i < gs->openingNovel.lineCount; i++)
    {
        free(gs->openingNovel.lines[i]); // strdupで確保したメモリを解放
    }
    free(gs->openingNovel.lines); // 行ポインタ配列自体を解放

    for (int i = 0; i < gs->endingNovel.lineCount; i++)
    {
        free(gs->endingNovel.lines[i]); // strdupで確保したメモリを解放
    }
    free(gs->endingNovel.lines); // 行ポインタ配列自体を解放
    // フォントとサウンドの解放
    TTF_CloseFont(gs->font);
    TTF_CloseFont(gs->largeFont);

    Mix_FreeChunk(gs->damageSound);
    Mix_FreeChunk(gs->veggieGetSound);
    Mix_FreeChunk(gs->gameOverSound);
    // SDLサブシステムの終了

    if (gs->ddrMat)
        SDL_JoystickClose(gs->ddrMat);
    SDL_DestroyRenderer(gs->renderer);
    SDL_DestroyWindow(gs->window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}