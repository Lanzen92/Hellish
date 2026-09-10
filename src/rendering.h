#pragma once

#include "SDL3/SDL_render.h"
#include "image.h"
#include "camera.h"


void RenderSpriteWorld(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1);
void RenderSpriteGrid(Image* sprite, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1);
