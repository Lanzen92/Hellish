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

  Sprite* tileset;
  switch (levelData->tileset->type) {
    case TILESETS::DUNGEON:
      tileset = GetSprite(SPRITE_ID::DungeonTileset, gameData->spriteBuffer);
      break;
    
    case TILESETS::NONE:
    case TILESETS::COUNT:
      assert(false);
      break;
  }
  
  for (int y = 0; y < levelData->w; y++) {
    for (int x = 0; x < levelData->h; x++) {
      uint8_t id = GetCellID(levelData, x, y);
      RenderTileWorld(tileset, id, levelData, renderer, &gameData->camera, x, y, 1, 1);
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
    if (entity->active == false) {
      continue;
    }

    Sprite* sprite = GetSpriteFromEntityState(entity, gameData->spriteBuffer);

    if (HasBehaviour(entity, Behaviour::IS_PETRIFIED)) {
      sprite = GetSpriteFromID(ENTITY_ID::ROCK, gameData->spriteBuffer);
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


