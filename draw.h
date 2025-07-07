#ifndef DRAW_H
#define DRAW_H

#include "structs.h"

// void DrawGameOverScene(GameState *gs);
// void DrawMainStage(GameState *gs);
void DrawGame(GameState *gs);
// void DrawMainStage(GameState *gs);
void DrawDifficultyScene(GameState *gs);

// void DrawTitleScene(GameState *gs);
// void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);
#endif // DRAW_H