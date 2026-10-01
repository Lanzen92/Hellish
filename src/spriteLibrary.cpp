#include <cassert>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "spriteLibrary.h"

#include "common.h"

#include <cmath>

using namespace std;

const char* FALLBACK_PATH = "assets/sprites/fallback.png";


static const SpriteDataEntry allSpriteData[] = {
  {   SPRITE_ID::Fallback,                  FALLBACK_PATH, 8, 8},
  {   SPRITE_ID::Demon,                "assets/sprites/player.png"},
  {   SPRITE_ID::Rock,                 "assets/sprites/rock.png", 10, 20 },
  {   SPRITE_ID::Medusa_Rotate,        "assets/sprites/medusa_rotate.png", 12, 24, 8, 1},
  {   SPRITE_ID::Medusa_Idle_Left,     "assets/sprites/medusa_idle_left.png", 12, 24, 4, 1, 8 },
  {   SPRITE_ID::Medusa_Idle_Front,    "assets/sprites/medusa_idle_front.png", 12,24, 4, 1, 8},
  {   SPRITE_ID::Medusa_Idle_Back,     "assets/sprites/medusa_idle_back.png", 12, 24, 4, 1, 8 },
  // {   SPRITE_ID::Golem,                "assets/sprites/golem.png" },
  {   SPRITE_ID::DropShadow,           "assets/sprites/dropshadow.png", 8, 8},
  {   SPRITE_ID::SelectionMarker,      "assets/sprites/selection_marker.png",9,9},
  {   SPRITE_ID::Siren,                "assets/sprites/siren.png"  },
  {   SPRITE_ID::TitleScreenBackground,"assets/sprites/titlescreen.png", 0,0  },
  {   SPRITE_ID::MainMenuBackground,   "assets/sprites/mainmenu_background.png", 0,0  },
  {   SPRITE_ID::Black1x1             ,"assets/sprites/1x1black.png", 0,0  },
  {   SPRITE_ID::DungeonTileset       ,"assets/sprites/hell_of_a_time_dungeon_tileset.png", 0,0, 9, 9  },
    //{   SPRITE_ID::Ghost,               "assets/sprites/ghost.png"     },
  {   SPRITE_ID::Goal,                 "assets/sprites/goal.png", 8, 8, 8, 1},
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

 SpriteRenderInfo GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer, const uint64_t* ticksTotal) {
    if (HasBehaviour(entity, IS_PETRIFIED)) {
      return GetSprite(SPRITE_ID::Rock, spriteBuffer);
    }

    if (entity->id == ENTITY_ID::MEDUSA && entity->action == Actions::ROTATING) {
      Sprite* spritesheet = GetSprite(SPRITE_ID::Medusa_Rotate, spriteBuffer);
    
      int start = 0;
      int end = 0;
      switch(entity->facingPrevious){
      case Direction::RIGHT:
        start = 6;
        break;
      case Direction::LEFT:
        start = 2;
        break;
      case Direction::UP:
        start = 4;
        break;
      case Direction::DOWN:
        start = 0;
        break;
      }
      switch(entity->facingCurrent){
      case Direction::RIGHT:
        end = 6;
        break;
      case Direction::LEFT:
        end = 2;
        break;
      case Direction::UP:
        end = 4;
        break;
      case Direction::DOWN:
        end = 0;
        break;
      }
        
      int spriteCount = GetSpriteCount(spritesheet);
      int forward = ((end - start) % spriteCount + spriteCount) % spriteCount;
      int backward = spriteCount - forward;
      end = (forward <= backward) ? (start + forward) : (start - backward);
      
      float exactFrame = std::lerp((float)start, (float)end, entity->progress01);
      int roundedFrame = (int)std::round(exactFrame);
      int currentFrame = ((roundedFrame % spriteCount) + spriteCount) % spriteCount;
      //int currentFrame = ((int)std::lerp(start, end, entity->progress01) % spriteCount);
      
      SDL_Log("Action: %d, Start: %d, End: %d, Progress: %f, Frame: %d\n", 
       (int)entity->action, start, end, entity->progress01, currentFrame);
      
      return {currentFrame, spritesheet};
    }
  
    switch (entity->id) {
    case ENTITY_ID::MEDUSA:{
      Sprite* sprite = nullptr;
      int frame = 0;
      switch (entity->facingCurrent) {
      case Direction::RIGHT:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Left, spriteBuffer);
        frame = (int)((*ticksTotal * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite, true};
      case Direction::LEFT:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Left, spriteBuffer);
        frame = (int)((*ticksTotal * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      case Direction::DOWN:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Back, spriteBuffer);
        frame = (int)((*ticksTotal * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      case Direction::UP:
        sprite = GetSprite(SPRITE_ID::Medusa_Idle_Front, spriteBuffer);
        frame = (int)((*ticksTotal * sprite->framerate) / FPS % GetSpriteCount(sprite));
        return {frame, sprite};
      }
      break;
    }
    case ENTITY_ID::DEMON:
      return GetSprite(SPRITE_ID::Demon, spriteBuffer);
    case ENTITY_ID::ROCK:
      return GetSprite(SPRITE_ID::Rock, spriteBuffer);
    default:
      return GetSprite(SPRITE_ID::Fallback, spriteBuffer);
  }  

  assert(false);
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
      sprite->framerate = entry.framerate;

      if (entry.pivotX == NOT_SET || entry.pivotY == NOT_SET) {
        sprite->pivotX = sprite->width / 2;
        sprite->pivotY = sprite->height / 2;
      } 
      else {
        sprite->pivotX = entry.pivotX;
        sprite->pivotY = entry.pivotY;
      }

      sprite->spriteCountX = entry.tilesetCellCountX;
      sprite->spriteCountY = entry.tilesetCellCountY;
      
      SDL_Log("ID %d | Texture: %p | W: %d | H: %d | PivotX: %d | PivotY: %d | Path: %s", entry.id,
                (void*)sprite->texture, sprite->width, sprite->height, sprite->pivotX, sprite->pivotY, entry.path);

      SDL_DestroySurface(surface);
    }
  }

  
