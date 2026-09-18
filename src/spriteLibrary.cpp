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
    {   SPRITE_ID::Golem,    "assets/sprites/golem.png"     },
    {   SPRITE_ID::Ghost,    "assets/sprites/ghost.png"     },
    {   SPRITE_ID::Siren,    "assets/sprites/siren.png"     },
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
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Medusa];
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

      SDL_Log("ID %d | Texture: %p | W: %d | H: %d | Path: %s", entry.id,
                (void*)sprite->texture, sprite->width, sprite->height, entry.path);

      SDL_DestroySurface(surface);
    }
  }

  
