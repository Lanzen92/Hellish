#pragma once

#include "SDL3/SDL_render.h"
#include "entity.h"

enum class SPRITE_ID {
  Fallback = 0,
  Wall = 1,
  Ground = 2,
  Demon = 3,
  Rock = 4,
  Medusa = 5,
  Golem = 6,
  Ghost = 7,
  Siren = 8
};

struct Sprite {
  SDL_Texture* texture;
  int width;
  int height;
};

struct SpriteDataEntry {
  SPRITE_ID id;
  const char* path;
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer);

namespace AssetManagement {
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);
}
