#include <cstdint>
#include <cmath>

#include "common.h"
#include "levelRenderer.h"
#include "rendering.h"

void RenderLevel(GameData* gameData, SDL_Renderer* renderer) {

  LevelData levelData = gameData->levels[gameData->currentLevelIndex];

  int boardWidthPxHalf = levelData.w * CELL_SIZE_PX / 2;
  int boardHeightPxHalf = levelData.h * CELL_SIZE_PX / 2;

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

      float xPos = x * CELL_SIZE_PX;
      float yPos = y * CELL_SIZE_PX;

      xPos += SCREEN_WIDTH / 2.0;
      yPos += SCREEN_HEIGHT / 2.0;

      xPos -= boardWidthPxHalf;
      yPos -= boardHeightPxHalf;

      RenderSprite(sprite, renderer, xPos, yPos);
      
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

    int xPos = 0;
    int yPos = 0;
     
    xPos += SCREEN_WIDTH / 2.0;
    yPos += SCREEN_HEIGHT / 2.0;

    xPos -= gameData->levels[gameData->currentLevelIndex].w * CELL_SIZE_PX / 2;
    yPos -= gameData->levels[gameData->currentLevelIndex].h * CELL_SIZE_PX / 2;

    float xAnimated = std::lerp(entity.xPrev, entity.x, entity.progress01);
    float yAnimated = std::lerp(entity.yPrev, entity.y, entity.progress01);
    
    xPos += xAnimated * CELL_SIZE_PX;
    yPos += yAnimated * CELL_SIZE_PX;

    RenderSprite(image, renderer, xPos, yPos);
       
  }
}
