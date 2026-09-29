#pragma once

#include "button.h"
#include "SDL3/SDL_render.h"

#include "camera.h"
#include "spriteLibrary.h"

void RenderSpriteWorld(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera* camera, 
			float x, float y, float scale = 1, float alpha = 1, bool flipped = false);

void RenderTile(Sprite* tileset, int cellId, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera, 
		float x, float y, float scale = 1, float alpha = 1);


void RenderSpriteOnTile(SpriteRenderInfo spriteInfo, LevelData* levelData, SDL_Renderer* renderer, 
		const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);

void RenderButton(Button* button, bool isSelected, SDL_Renderer* renderer);