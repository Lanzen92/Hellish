#include <cstdint>
#include <cmath>

#include "common.h"
#include "levelRenderer.h"
#include "rendering.h"

void RenderLevel(GameData* gameData, SDL_Renderer* renderer) {

  LevelData levelData = gameData->levels[gameData->currentLevelIndex];

  for (int x = 0; x < levelData.w; x++) {
    for (int y = 0; y < levelData.h; y++) {
      uint8_t cellType = levelData.GetCell(x, y);
      Image* sprite;

      switch((ID)cellType) {
        case ID::GROUND:
          sprite = gameData->ground;
          break;

        case ID::WALL:
          sprite = gameData->wall;
          break;

        default:
          sprite = gameData->fallback;
          break;
      }

      RenderSpriteGrid(sprite, &levelData, renderer, &gameData->camera, x, y);
        
    }
  }
}

void RenderEntities(GameData* gameData, SDL_Renderer* renderer) {

  LevelData levelData = gameData->levels[gameData->currentLevelIndex];

  for (int i = 0; i < levelData.entityCount; i++) {
    Image* image;
    Entity entity = levelData.entityBuffer[i];

    switch(entity.id) {
      case ID::PLAYER:
        image = gameData->player;
        break;
      case ID::BOX:
        image = gameData->box;
        break;
      default:
        image = gameData->fallback;
        break;
    }

    float xAnimated = std::lerp(entity.xPrev, entity.x, entity.progress01);
    float yAnimated = std::lerp(entity.yPrev, entity.y, entity.progress01);
    
    RenderSpriteGrid(image, &levelData, renderer, &gameData->camera, xAnimated, yAnimated);
       
  }
}
