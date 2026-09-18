#pragma once

#include <cstdint>
#include "arena.h"
#include "entity.h"

using namespace Memory;

struct LevelData {
  int w;
  int h;
  uint8_t* cells;
  const char* levelPath;
  Entity* entityBuffer;
  int entityCount;

};

void CreateLevel(Arena* arena, LevelData* levelData, const char* levelName);
void CreateEntities(LevelData* levelData, Arena* arena);

Entity* GetNextAvailableEntitySlot(LevelData* levelData);

void AddEntity(ID entity, int x, int y, LevelData* levelData);

void RemoveEntity(int x, int y, LevelData* levelData);

uint8_t GetCell(LevelData* levelData,int x, int y);

Entity* GetEntity(LevelData* levelData,int x, int y);

Entity* RaycastFirstEntity(int xOrigin, int yOrigin, Direction direction, LevelData* levelData, bool ignoreWalls = false);
