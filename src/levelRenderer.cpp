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
      uint16_t id = GetCellID(levelData, x, y);
      RenderTile(tileset, id, levelData, renderer, &gameData->camera, x, y, 1, 1);
    }
  }
  
  for(int i = 0; i < levelData->goalCount; i++){
    Goal goal = levelData->goals[i];
    Sprite* sprite = GetSprite(SPRITE_ID::Goal, gameData->spriteBuffer);
    int frame = (int)(goal.blinkTimer / 0.2) % (sprite->spriteCountX * sprite->spriteCountY);
    RenderSpriteOnTile({frame, sprite}, levelData, renderer, &gameData->camera, goal.x, goal.y);
  }
  
}

void RenderEntities(GameData* gameData, SDL_Renderer* renderer) {

  LevelData* levelData = &gameData->scenes.gameplay.levels[gameData->scenes.gameplay.currentLevelIndex];

  Entity** sortedEntities = ALLOC_ARRAY(gameData->arenaScratch, Entity*, levelData->entityCount)
  for (int i = 0; i < levelData->entityCount; i++) { 
    sortedEntities[i] = &levelData->entityBuffer[i];
  }
  std::sort(sortedEntities, sortedEntities + levelData->entityCount, IsEntityBelowOtherEntity);
  
  Gameplay* gameplay = &gameData->scenes.gameplay;
  Entity* activeEntity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex];
  
  for (int i = 0; i < levelData->entityCount; i++) {
    Entity* entity = sortedEntities[i];
    if (entity->active == false) {
      continue;
    }

    SpriteRenderInfo sprite = GetSpriteFromEntityState(entity, gameData->spriteBuffer, gameData->ticksTotal);

    if (HasBehaviour(entity, Behaviour::IS_PETRIFIED)) {
      sprite = GetSpriteFromID(ENTITY_ID::ROCK, gameData->spriteBuffer);
    }
  
    float xAnimated = std::lerp(entity->xPrev, entity->x, entity->progress01);
    float yAnimated = std::lerp(entity->yPrev, entity->y, entity->progress01);

    float groundY = yAnimated;

    if (HasBehaviour(entity, Behaviour::JUMPS) && !HasBehaviour(entity, Behaviour::IS_PUSHING)) {
      yAnimated -= 0.5 * sinf(entity->progress01 * 3.14);
    }
    
    Sprite* dropshadow = &gameData->spriteBuffer[(int)SPRITE_ID::DropShadow];
    RenderSpriteOnTile(dropshadow, levelData, renderer, &gameData->camera, xAnimated, groundY, 1, 0.4, false);
    
    if (entity == activeEntity) {
      SpriteRenderInfo selection_marker = GetSprite(SPRITE_ID::SelectionMarker, gameData->spriteBuffer);  
      RenderSpriteOnTile(selection_marker, levelData, renderer, &gameData->camera, xAnimated, groundY);
    }
    
    RenderSpriteOnTile(sprite, levelData, renderer, &gameData->camera,xAnimated, yAnimated, 1, 1, false);
  }
}


