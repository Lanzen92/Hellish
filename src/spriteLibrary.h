#pragma once

#include "SDL3/SDL_render.h"

#include "entity.h"

const int NOT_SET = -1;

enum class SPRITE_ID {
  Fallback,
  Rock,
  Demon,
  Medusa_Idle_Side,
  Medusa_Idle_Front,
  Medusa_Idle_Back,
  Golem,
  Siren,
  DropShadow,
  TitleScreenBackground,
  Black1x1,
  DungeonTileset
};

struct Sprite {
  SDL_Texture* texture;
  int width;
  int height;
  int pivotX;
  int pivotY;
  int tilesetCellCountX;
  int tilesetCellCountY;
};

struct SpriteDataEntry {
  SPRITE_ID id;
  const char* path;
  int pivotX = NOT_SET;
  int pivotY = NOT_SET;
  int tilesetCellCountX = NOT_SET;
  int tilesetCellCountY = NOT_SET;
};

Sprite* GetSprite(SPRITE_ID spriteId, Sprite* spriteBuffer);
Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer);
Sprite* GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer);

namespace AssetManagement {
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);

}
