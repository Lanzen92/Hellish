#pragma once

#include "SDL3/SDL_render.h"
#include "entity.h"

const int NOT_SET = -1;

//TODO Something wrong when loading after changes with medusa etc.
enum class SPRITE_ID {
  Fallback,
  Ground,
  Ground_alt,
  Wall,
  Rock,
  Demon,
  Medusa_Idle_Side,
  Medusa_Idle_Front,
  Medusa_Idle_Back,
  Golem,
  Ghost,
  Siren,
  DropShadow
};

struct Sprite {
  SDL_Texture* texture;
  int width;
  int height;
  int pivotx;
  int pivoty;
};

struct SpriteDataEntry {
  SPRITE_ID id;
  const char* path;
  int pivotX = NOT_SET;
  int pivotY = NOT_SET;
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer);
Sprite* GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer);

namespace AssetManagement {
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);

}
