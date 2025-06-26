#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// ゲーム全体の状態を管理する構造体
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Joystick *ddrMat;
    bool isRunning;
} GameState;

#endif // STRUCTS_H