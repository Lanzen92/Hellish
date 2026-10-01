#include <cstdint>
#include <fstream>
#include <vector>

#include "Parsers/json.hpp"
#include "SDL3_image/SDL_image.h"

#include "arena.h"
#include "level.h"

#include "common.h"
#include "entity.h"
#include "tilesetLibrary.h"

using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;

namespace AssetManagement {

  std::vector<uint16_t> GetCellDataFromJsonLayer(nlohmann::json& parsedJson, const char* layerName, bool* wasFound) {
    std::vector<uint16_t> result;
    *wasFound = false;
    for (const auto& layer : parsedJson["layers"]) {
      if (layer["name"] == layerName) {
        result = layer["data"].get<vector<uint16_t>>();
        *wasFound = true;
        break;
      }
    }
    return result;
  }

  int GetFirstNonZeroCell(std::vector<uint16_t> *list){
    for (int id : *list) {
      if(id != 0){
        return id;
      }
    }
    assert(false);
    return -1;
  }
}

void CreateLevel(Arena* arena, LevelData* levelData, Tileset* tileset, const char* levelName) {
  fstream stream(levelName);
  auto jsonResult = nlohmann::json::parse(stream);
  
  bool found = false;
  vector<uint16_t> levelDataFromJson = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "level", &found);
  
  assert(found);
  
  int firstNonZeroId = AssetManagement::GetFirstNonZeroCell(&levelDataFromJson);
  int offsetId = GetTilesetIDOffsetFromTilemap(firstNonZeroId, jsonResult);
  
  levelData->w = jsonResult["width"].get<int>();
  levelData->h = jsonResult["height"].get<int>();
  levelData->levelPath = levelName;
  levelData->tileset = tileset;
  levelData->cells = ALLOC_ARRAY(arena, uint16_t, levelData->w * levelData->h)
  
   for (int i = 0; i < levelData->w * levelData->h; i++) {
     int localId = levelDataFromJson[i] - offsetId;
     if (localId < 0) {
       localId = 0;
     }
     
     levelData->cells[i] = localId;
   }
  
  vector<uint16_t> onLevel = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "on_level", &found);
  
  if (found) {
    int firstNonZero = AssetManagement::GetFirstNonZeroCell(&onLevel);
    offsetId = GetTilesetIDOffsetFromTilemap(firstNonZero, jsonResult);
    levelData->goalCount = 0;
    
    for (int i = 0; i < levelData->w * levelData->h; i++) {
      int localId = onLevel[i] - offsetId + 1;
      if (localId > 0) {
        levelData->goalCount++;
      }
    }
    
    levelData->goals = ALLOC_ARRAY(arena, Goal, levelData->goalCount)
    int index = 0;
    
    for (int i = 0; i < levelData->w * levelData->h; i++) {
      int localId = onLevel[i] - offsetId + 1;
      if (localId < 0) {
        localId = 0;
      }
      if (localId != 0) {
        int x;
        int y;
        Expand1DTo2D(i, levelData->w, &x, &y);
        levelData->goals[index].x = x;
        levelData->goals[index].y = y;
        index++;
      }
    }
  }
}

void CreateEntities(LevelData* levelData, Arena* arena) {
  Reset(arena);

  levelData->entityCount = 0;
  levelData->entityBuffer = ALLOC_ARRAY(arena, Entity, 256)
  
  fstream stream(levelData->levelPath);
  auto jsonResult = json::parse(stream);
  bool found = false;
  
  vector<uint16_t> entities = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "entities", &found);
  
  if(!found){
    return;
  }
  
  for (int i = 0; i < levelData->w * levelData->h; i++) {
    if(entities[i] == 0){
      continue;
    }
    
    uint16_t entity_id = GetLocalTileID(entities[i], jsonResult) + 1;
    int x;
    int y;
    Expand1DTo2D(i, levelData->w, &x, &y);
    AddEntity((ENTITY_ID)entity_id, x, y, levelData);
  }
}

Entity* GetNextAvailableEntity(LevelData* levelData) {
  for (int i = 0; i < levelData->entityCount; i++) {
    if (levelData->entityBuffer[i].active == false) {
      return &levelData->entityBuffer[i];
    }
  }

  return &levelData->entityBuffer[levelData->entityCount++];
}

void AddEntity(ENTITY_ID entityId, int x, int y, LevelData* levelData) {
  Entity* entity = GetEntity(levelData, x, y);

  if (entity == nullptr) {
    entity = GetNextAvailableEntity(levelData);
  }

  entity->active = true;
  entity->x = x;
  entity->y = y;
  entity->xPrev = x;
  entity->yPrev = y;
  entity->id = entityId;
  entity->action = Actions::NONE;
  InitializeBaseBehaviour(entity);
}

void RemoveEntity(int x, int y, LevelData* levelData) {
  Entity* entity = GetEntity(levelData, x, y);

  if (entity == nullptr) {
    return;
  }

  *entity = {};
}

uint16_t GetCellID(LevelData* levelData, int x, int y) { 
  return levelData->cells[y * levelData->w + x]; 
}

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
    ENTITY_ID cellID = (ENTITY_ID)GetCellID(levelData, xSearch, ySearch);

    if (!ignoreWalls && IsWalkable(xSearch, ySearch, levelData)) {
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

bool IsWalkable(int x, int y, LevelData* levelData) {
  uint16_t id = GetCellID(levelData, x, y);
  return levelData->tileset->walkableBuffer[id];
}