#include <cassert>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "spriteLibrary.h"

const char* FALLBACK_PATH = "assets/sprites/fallback.png";
// const char* ASSETS_SPRITES_PATH = "assets/sprites/";

static const SpriteDataEntry allSpriteData[] = {
    {   SPRITE_ID::Fallback, FALLBACK_PATH                  },
    {   SPRITE_ID::Wall,     "assets/sprites/wall.png"      },
    {   SPRITE_ID::Ground,   "assets/sprites/ground.png"    },
    {   SPRITE_ID::Demon,    "assets/sprites/player.png"    },
    {   SPRITE_ID::Rock,     "assets/sprites/box.png"       },
    {   SPRITE_ID::Medusa,   "assets/sprites/medusa.png"    },
    {   SPRITE_ID::Golem,   "assets/sprites/golem.png"      },
    {   SPRITE_ID::Ghost,   "assets/sprites/ghost.png"      },
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer) {
  
  switch(id) {
    case ID::NONE:
      return nullptr;
      
    case ID::GROUND:
      return &spriteBuffer[(int)SPRITE_ID::Ground];
      break;

    case ID::WALL:
      return &spriteBuffer[(int)SPRITE_ID::Wall];
      break;

    case ID::DEMON:
      return &spriteBuffer[(int)SPRITE_ID::Demon];
      break;

    case ID::ROCK:
      return &spriteBuffer[(int)SPRITE_ID::Rock];
      break;

    case ID::MEDUSA:
      return &spriteBuffer[(int)SPRITE_ID::Medusa];
      break;

    case ID::GHOST:
      return &spriteBuffer[(int)SPRITE_ID::Ghost];
      break;

    case ID::GOLEM:
      return &spriteBuffer[(int)SPRITE_ID::Golem];
      break;
      
    default:
      return &spriteBuffer[(int)SPRITE_ID::Fallback];
      break;
    }
  }

  namespace AssetManagement {
    
    void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer) {
      for (SpriteDataEntry entry : allSpriteData) {
        AssetManagement::LoadSprite(spriteBuffer, entry, renderer);
        
      }
    }

    void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer) {

      SDL_Surface* surface = IMG_Load(entry.path);
      if (surface == nullptr) {
        surface = IMG_Load(FALLBACK_PATH);
      }
      
      assert(surface != nullptr);

      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      Sprite* sprite = &spriteBuffer[(int)entry.id];
      sprite->texture = texture;
      sprite->height = texture->h;
      sprite->width = texture->w;

      SDL_DestroySurface(surface);
    }
  }

  
