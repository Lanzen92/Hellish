#include <cstdint>
#include <cmath>
#include <cstdio>
#include <string>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "common.h"
#include "levelRenderer.h"
#include "rendering.h"
#include "spriteLibrary.h"

void RenderLevel(GameData* gameData, SDL_Renderer* renderer) {

  LevelData levelData = gameData->levels[gameData->currentLevelIndex];

  for (int x = 0; x < levelData.w; x++) {
    for (int y = 0; y < levelData.h; y++) {
      uint8_t cellType = levelData.GetCell(x, y);
      
      if ((ID)cellType == ID::NONE) {
        continue;
      }

      Sprite* sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      RenderSpriteGrid(sprite, &levelData, renderer, &gameData->camera, x, y);  
    }
  }
}

void RenderEntities(GameData* gameData, SDL_Renderer* renderer) {

  LevelData levelData = gameData->levels[gameData->currentLevelIndex];

  for (int i = 0; i < levelData.entityCount; i++) {

    Entity entity = levelData.entityBuffer[i];

    if (entity.id == ID::NONE) {
      continue;
    }

    Sprite* sprite = GetSpriteFromID(entity.id, gameData->spriteBuffer);
  
    float xAnimated = std::lerp(entity.xPrev, entity.x, entity.progress01);
    float yAnimated = std::lerp(entity.yPrev, entity.y, entity.progress01);
    
    RenderSpriteGrid(sprite, &levelData, renderer, &gameData->camera, xAnimated, yAnimated);   
  }
}
