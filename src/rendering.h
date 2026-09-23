#pragma once

#include "SDL3/SDL_render.h"

#include "camera.h"
#include "spriteLibrary.h"

void RenderSpriteWorld(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera, 
		float x, float y, float scale = 1, float alpha = 1, bool flipped = false);

void RenderTileWorld(Sprite* tilesetAtlasSprite, int cellId, LevelData* leveldata, SDL_Renderer* renderer, 
		const Camera* camera, float x, float y, float scale, float alpha);

void RenderEntityOnTile(Sprite* sprite, LevelData* levelData, SDL_Renderer* renderer, 
		const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);