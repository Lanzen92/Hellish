#pragma once

#include "SDL3/SDL_render.h"

#include "entity.h"

const int NOT_SET = -1;

enum class SPRITE_ID {
  Fallback,
  Rock,
  Demon,
  Medusa_Rotate,
  // Medusa_Idle_Side,
  // Medusa_Idle_Front,
  // Medusa_Idle_Back,
  Golem,
  Siren,
  DropShadow,
  TitleScreenBackground,
  MainMenuBackground,
  Black1x1,
  DungeonTileset,
  SelectionMarker,
  Goal,
};

struct Sprite {
  SDL_Texture* texture;
  int width;
  int height;
  int pivotX;
  int pivotY;
  int spriteCountX;
  int spriteCountY;
};

struct SpriteDataEntry {
  SPRITE_ID id;
  const char* path;
  int pivotX = NOT_SET;
  int pivotY = NOT_SET;
  int tilesetCellCountX = NOT_SET;
  int tilesetCellCountY = NOT_SET;
};

struct SpriteRenderInfo{
  Sprite* sprite;
  int frame;
  SpriteRenderInfo(){
    this->sprite = nullptr;
    this->frame = 0;
  }
  SpriteRenderInfo(int frame, Sprite* sprite){
    this->frame = frame;
    this->sprite = sprite;
  }
  SpriteRenderInfo(Sprite* sprite){
    this->sprite = sprite;
    this->frame = 0;
  }
};

inline int GetSpriteCount(Sprite* sprite){
  if(sprite->spriteCountX == NOT_SET) return 1;
  if(sprite->spriteCountY == NOT_SET) return 1;
  return sprite->spriteCountX * sprite->spriteCountY;
}

Sprite* GetSprite(SPRITE_ID spriteId, Sprite* spriteBuffer);
Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer);
SpriteRenderInfo GetSpriteFromEntityState(Entity* entity, Sprite* spritebuffer);

namespace AssetManagement {
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);

}
