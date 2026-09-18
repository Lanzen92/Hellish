#include <cassert>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "spriteLibrary.h"

const char* FALLBACK_PATH = "assets/sprites/fallback.png";
// const char* ASSETS_SPRITES_PATH = "assets/sprites/";
// TODO Something wrong when loading after changes with medusa etc.
static const SpriteDataEntry allSpriteData[] = {
    {   SPRITE_ID::Fallback,            FALLBACK_PATH },
    {   SPRITE_ID::Ground,              "assets/sprites/ground.png", 0, 0 },
    {   SPRITE_ID::Ground_alt,          "assets/sprites/ground_alt.png", 0, 0 },  
    {   SPRITE_ID::Wall,                "assets/sprites/wall.png", 0,0 },
    {   SPRITE_ID::Rock,                "assets/sprites/rock.png", 10, 20 },
    {   SPRITE_ID::Demon,               "assets/sprites/player.png" },
    {   SPRITE_ID::Medusa_Idle_Side,    "assets/sprites/medusa_idle_side.png", 12, 24 },
    {   SPRITE_ID::Medusa_Idle_Front,   "assets/sprites/medusa_idle_front.png", 12,24 },
    {   SPRITE_ID::Medusa_Idle_Back,    "assets/sprites/medusa_idle_back.png", 12, 24 },
    {   SPRITE_ID::Golem,               "assets/sprites/golem.png" },
    {   SPRITE_ID::DropShadow,          "assets/sprites/dropshadow.png", 8, 8},
    //{   SPRITE_ID::Siren,             "assets/sprites/siren.png"     },
    //{   SPRITE_ID::Ghost,             "assets/sprites/ghost.png"     },
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer) {
  Sprite* spriteToReturn = nullptr;

  switch(id) {
    case ID::NONE:
      spriteToReturn = nullptr;
      
    case ID::GROUND:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Ground];
      break;

    case ID::WALL:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Wall];
      break;

    case ID::DEMON:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Demon];
      break;

    case ID::ROCK:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Rock];
      break;

    case ID::MEDUSA:
      spriteToReturn = nullptr;
      break;

    case ID::GHOST:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Ghost];
      break;

    case ID::GOLEM:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Golem];
      break;

    case ID::SIREN:
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
      case ID::MEDUSA:
        switch (entity->facing) { 
        case Direction::RIGHT:
        case Direction::LEFT:
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Side];
          break;
        case Direction::UP: 
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Back];
          break;
        case Direction::DOWN:
          return &spriteBuffer[(int)SPRITE_ID::Medusa_Idle_Front];
          break;
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

      if (sprite->pivotx == NOT_SET || sprite->pivoty == NOT_SET) {
        sprite->pivotx = sprite->width / 2;
        sprite->pivoty = sprite->height / 2;
      } 
      else {
        sprite->pivotx = entry.pivotX;
        sprite->pivoty = entry.pivotY;
      }

      SDL_Log("ID %d | Texture: %p | W: %d | H: %d | Path: %s", entry.id,
                (void*)sprite->texture, sprite->width, sprite->height, entry.path);

      SDL_DestroySurface(surface);
    }
  }

  
