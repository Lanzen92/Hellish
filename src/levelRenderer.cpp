#include <cstdint>

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

      switch(cellType) {
        case 1:
          sprite = gameData->ground;
          break;

        case 2:
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
      case 3:
        image = gameData->player;
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

    xPos += entity.x * CELL_SIZE_PX;
    yPos += entity.y * CELL_SIZE_PX;

    RenderSprite(image, renderer, xPos, yPos);
       
  }
}
