#include <cstdint>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "common.h"
#include "levelRenderer.h"
#include "rendering.h"
#include "spriteLibrary.h"

bool IsEntityBelowOtherEntity(Entity* a, Entity* b) { return a->y < b->y; }

void RenderLevel(GameData* gameData, SDL_Renderer* renderer) {

  Gameplay* gameplay = &gameData->scenes.gameplay;
  LevelData* levelData = &gameplay->levels[gameplay->currentLevelIndex];

  for (int x = 0; x < levelData->w; x++) {
    for (int y = 0; y < levelData->h; y++) {
      uint8_t cellType = GetCellID(levelData, x, y);
      
      if ((ID)cellType == ID::NONE) {
        continue;
      }

      Sprite* sprite;
      if (ID(cellType) == ID::GROUND) {
        sprite = &gameData->spriteBuffer[(x + y) % 2 == 0 ? (int)SPRITE_ID::Ground : (int)SPRITE_ID::Ground_alt];
      } 
      else {
        sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      }

      RenderSpriteGrid(sprite, levelData, renderer, &gameData->camera, x, y);  
    }
  }
}

void RenderEntities(GameData* gameData, SDL_Renderer* renderer) {

  LevelData* levelData = &gameData->scenes.gameplay.levels[gameData->scenes.gameplay.currentLevelIndex];

  Entity** sortedEntities = ALLOC_ARRAY(gameData->arenaScratch, Entity*, levelData->entityCount)
  for (int i = 0; i < levelData->entityCount; i++) { 
    sortedEntities[i] = &levelData->entityBuffer[i];
  }
  std::sort(sortedEntities, sortedEntities + levelData->entityCount, IsEntityBelowOtherEntity);

  for (int i = 0; i < levelData->entityCount; i++) {

    Entity* entity = sortedEntities[i];

    if (entity->id == ID::NONE) {
      continue;
    }

    Sprite* sprite = GetSpriteFromEntityState(entity, gameData->spriteBuffer);

    if (HasBehaviour(entity, Behaviour::IS_PETRIFIED)) {
      sprite = GetSpriteFromID(ID::ROCK, gameData->spriteBuffer);
    }
  
    float xAnimated = std::lerp(entity->xPrev, entity->x, entity->progress01);
    float yAnimated = std::lerp(entity->yPrev, entity->y, entity->progress01);

    float dropshadowY = yAnimated;

    if (HasBehaviour(entity, Behaviour::JUMPS) && !HasBehaviour(entity, Behaviour::IS_PUSHING)) {
      yAnimated -= 0.5 * sinf(entity->progress01 * 3.14);
    }
    
    Sprite* dropshadow = &gameData->spriteBuffer[(int)SPRITE_ID::DropShadow];
    RenderEntityOnTile(dropshadow, levelData, renderer, &gameData->camera, xAnimated, dropshadowY, 1, 0.4, false);
    RenderEntityOnTile(sprite, levelData, renderer, &gameData->camera,xAnimated, yAnimated, 1, 1, entity->facing == Direction::RIGHT);
  }
}


