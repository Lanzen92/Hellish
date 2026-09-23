#include <cassert>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "spriteLibrary.h"

const char* FALLBACK_PATH = "assets/sprites/fallback.png";

static const SpriteDataEntry allSpriteData[] = {
  {   SPRITE_ID::Fallback,            FALLBACK_PATH, 0, 0},
  {   SPRITE_ID::Rock,                 "assets/sprites/rock.png", 10, 20 },
  {   SPRITE_ID::Demon,                "assets/sprites/player.png"},
  {   SPRITE_ID::Medusa_Idle_Side,     "assets/sprites/medusa_idle_side.png", 12, 24 },
  {   SPRITE_ID::Medusa_Idle_Front,    "assets/sprites/medusa_idle_front.png", 12,24 },
  {   SPRITE_ID::Medusa_Idle_Back,     "assets/sprites/medusa_idle_back.png", 12, 24 },
  {   SPRITE_ID::Golem,                "assets/sprites/golem.png" },
  {   SPRITE_ID::DropShadow,           "assets/sprites/dropshadow.png", 8, 8},
  {   SPRITE_ID::Siren,                "assets/sprites/siren.png"  },
  {   SPRITE_ID::TitleScreenBackground,"assets/sprites/titlescreen.png", 0,0  },
  {   SPRITE_ID::Black1x1             ,"assets/sprites/1x1black.png", 0,0  },
  {   SPRITE_ID::DungeonTileset       ,"assets/sprites/hell_of_a_time_dungeon_tileset.png", 0,0, 9, 9  },
    //{   SPRITE_ID::Ghost,               "assets/sprites/ghost.png"     },
};

Sprite* GetSprite(SPRITE_ID spriteId, Sprite* spriteBuffer) {
  return &spriteBuffer[(int) spriteId];
}

Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer) {
  Sprite* spriteToReturn = nullptr;

  switch(id) {

    case ENTITY_ID::DEMON:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Demon];
      break;

    case ENTITY_ID::ROCK:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Rock];
      break;

    case ENTITY_ID::MEDUSA:
      spriteToReturn = nullptr;
      break;
  
    case ENTITY_ID::GOLEM:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Golem];
      break;

    case ENTITY_ID::SIREN:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Siren];
      break;
      
    }

    if (spriteToReturn == nullptr || spriteToReturn->texture == nullptr) {
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Fallback];
    }

    return spriteToReturn;
  }

  Sprite* GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer) {
    if (HasBehaviour(entity, IS_PETRIFIED)) {
      return &spriteBuffer[(int)SPRITE_ID::Rock];
    }

    switch (entity->id) {
      case ENTITY_ID::MEDUSA:
        switch (entity->facing) { 
        case Direction::RIGHT:
        case Direction::LEFT:
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Side];
        case Direction::DOWN: 
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Back];
        case Direction::UP:
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Front];
        }
      default:
        return GetSpriteFromID(entity->id, spriteBuffer);
        break;
    }
      return nullptr;
  }

  namespace AssetManagement {
    void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer) {
      SDL_Log("=========== LOADED TEXTURES ===========");

      for (SpriteDataEntry entry : allSpriteData) {
        LoadSprite(spriteBuffer, entry, renderer);
      }
    }

    void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer) {

      SDL_Surface* surface = IMG_Load(entry.path);
      if (surface == nullptr) {
        surface = IMG_Load(FALLBACK_PATH);
      }
      
      assert(surface != nullptr);

      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
      Sprite* sprite = &spriteBuffer[(int)entry.id];
      sprite->texture = texture;
      sprite->height = texture->h;
      sprite->width = texture->w;

      if (entry.pivotX == NOT_SET || entry.pivotY == NOT_SET) {
        sprite->pivotX = sprite->width / 2;
        sprite->pivotY = sprite->height / 2;
      } 
      else {
        sprite->pivotX = entry.pivotX;
        sprite->pivotY = entry.pivotY;
      }

      sprite->tilesetCellCountX = entry.tilesetCellCountX;
      sprite->tilesetCellCountY = entry.tilesetCellCountY;
      
      SDL_Log("ID %d | Texture: %p | W: %d | H: %d | PivotX: %d | PivotY: %d | Path: %s", entry.id,
                (void*)sprite->texture, sprite->width, sprite->height, sprite->pivotX, sprite->pivotY, entry.path);

      SDL_DestroySurface(surface);
    }
  }

  
