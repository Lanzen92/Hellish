#include <cstdint>
#include <fstream>
#include <vector>
#include "Parsers/json.hpp"

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

  for (int i = 0; i < levelData->w * levelData->h; i++) {
    unsigned char entityId = entityData[i];
    if (entityId != 0) {
      levelData->entityCount++;
    }
  }
  
  levelData->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * levelData->entityCount);

  int index = 0;
  for (int i = 0; i < levelData->w * levelData->h; i++) {
    unsigned char entityId = entityData[i];

    if (entityId != 0) {
      int x = i % levelData->w;
      int y = i / levelData->w;

      levelData->entityBuffer[index].id = (ID)entityId;
      levelData->entityBuffer[index].InitializeBaseBehaviour();
      levelData->entityBuffer[index].x = x;
      levelData->entityBuffer[index].y = y;

      index++;  
    }
  }
}
