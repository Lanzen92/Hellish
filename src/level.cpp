#include <cstdint>
#include <fstream>
#include <vector>

#include "Parsers/json.hpp"
#include "SDL3_image/SDL_image.h"

#include "arena.h"
#include "level.h"
#include "entity.h"


using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;

void CreateLevel(Arena* arena, LevelData* levelData, const char* levelName) {
  fstream stream(levelName);
  auto jsonResult = nlohmann::json::parse(stream);
  vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();

  levelData->w = jsonResult["width"].get<int>();
  levelData->h = jsonResult["height"].get<int>();
  levelData->levelPath = levelName;
  
  size_t sizeOfCells = sizeof(uint8_t) * levelData->w * levelData->h;
  levelData->cells = (uint8_t*)Memory::Allocate(arena, sizeOfCells);

  for (int i = 0; i < levelData->w * levelData->h; i++) {
    levelData->cells[i] = dataField[i];
  }
}

void CreateEntities(LevelData* levelData, Arena* arena) {
  Reset(arena);

  levelData->entityCount = 0;
  fstream stream(levelData->levelPath);

  auto result = nlohmann::json::parse(stream);
  auto entityData = result["layers"][ENTITIES_INDEX]["data"].get<vector<uint8_t>>();

  levelData->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * 256);

  for (int i = 0; i < levelData->w * levelData->h; i++) {
    unsigned char entityId = entityData[i];

    if (entityId != 0) {
      int x = i % levelData->w;
      int y = i / levelData->w;
      AddEntity((ID) entityId, x, y, levelData);
    }
  }
}

Entity* GetNextAvailableEntity(LevelData* levelData) {
  for (int i = 0; i < levelData->entityCount; i++) {
    if (levelData->entityBuffer[i].id == ID::NONE) {
      return &levelData->entityBuffer[i];
    }
  }

  return &levelData->entityBuffer[levelData->entityCount++];
}

void AddEntity(ID entityId, int x, int y, LevelData* levelData) {
  Entity* entity = GetEntity(levelData, x, y);

  if (entity == nullptr) {
    entity = GetNextAvailableEntity(levelData);
  }

  entity->x = x;
  entity->y = y;
  entity->xPrev = x;
  entity->yPrev = y;
  entity->id = entityId;
  InitializeBaseBehaviour(entity);
}

void RemoveEntity(int x, int y, LevelData* levelData) {
  Entity* entity = GetEntity(levelData, x, y);

  if (entity == nullptr) {
    return;
  }

  *entity = {};
}

uint8_t GetCell(LevelData* levelData, int x, int y) { return levelData->cells[y * levelData->w + x]; }

Entity* GetEntity(LevelData* levelData, int x, int y) {
  for (int i = 0; i < levelData->entityCount; i++) {
    if (levelData->entityBuffer[i].x == x && levelData->entityBuffer[i].y == y) {
      return &levelData->entityBuffer[i];
    }
  }

  return nullptr;
}

Entity* RaycastFirstEntity(int xOrigin, int yOrigin, Direction direction, LevelData* levelData, bool ignoreWalls) {
  Position facingVector;

  switch (direction) {
  case Direction::RIGHT: 
    facingVector = {1, 0};
    break;
  
  case Direction::LEFT:
    facingVector = {1, 0};
    break;
  
  case Direction::UP:
    facingVector = {0, 1};
    break;

  case Direction::DOWN:
    facingVector = {0, -1};
    break;
  }

  int xSearch = xOrigin + facingVector.x;
  int ySearch = yOrigin + facingVector.y;

  while (xSearch > 0 && xSearch < levelData->w && ySearch > 0 &&
         ySearch < levelData->h) {
    ID cellID = (ID)GetCell(levelData, xSearch, ySearch);

    if (cellID == ID::WALL && !ignoreWalls) {
      break;
    }

    Entity* entitySearch = GetEntity(levelData, xSearch, ySearch);
    if (entitySearch != nullptr) {
      return entitySearch;
    }

    xSearch += facingVector.x;
    ySearch += facingVector.y;
  }

  return nullptr;
}