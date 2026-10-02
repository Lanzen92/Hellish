#pragma once

#include "SDL3/SDL_render.h"

#include "camera.h"
#include "spriteLibrary.h"
#include "fontLibrary.h"

struct Button;
struct FontAtlas;

enum class Alignment {
	Right,
	Centered
};

enum class Type {
	Normal,
	Header
};

void RenderSpriteWorld(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera* camera, 
			float x, float y, float scale = 1, float alpha = 1, bool flipped = false);

void RenderTile(Sprite* tileset, int cellId, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera, 
		float x, float y, float scale = 1, float alpha = 1);


void RenderSpriteOnTile(SpriteRenderInfo spriteInfo, LevelData* levelData, SDL_Renderer* renderer, 
		const Camera* camera, float x, float y, float scale = 1, float alpha = 1, bool flipped = false);

void RenderButton(Button* button, bool isSelected, SDL_Renderer* renderer);

void RenderBackground(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, float alpha = 1.0f, bool flipped = false);

void RenderText(FontAtlas* atlas, const char* text, SDL_Renderer* renderer, Camera* camera, const float x, const float y, Alignment alignment, Type type = Type::Normal);