#ifndef INIT_H
#define INIT_H

#include "structs.h"

bool InitGame(GameState *gs);
void LoadAssets(GameState *gs);
void Cleanup(GameState *gs);

#endif // INIT_H