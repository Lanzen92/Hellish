#pragma once

#include <cstdint>

#include "arena.h"
#include "entity.h"
#include "tilesetLibrary.h"

using namespace Memory;

struct LevelData {
  int w;
  int h;
  uint16_t* cells;
  const char* levelPath;
  Entity* entityBuffer;
  int entityCount;
  const Tileset* tileset;

};

void CreateLevel(Arena* arena, LevelData* levelData, Tileset* tileset, const char* levelName);
void CreateEntities(LevelData* levelData, Arena* arena);

Entity* GetNextAvailableEntitySlot(LevelData* levelData);
void AddEntity(ENTITY_ID entity, int x, int y, LevelData* levelData);
void RemoveEntity(int x, int y, LevelData* levelData);

uint16_t GetCellID(LevelData* levelData,int x, int y);
Entity* GetEntity(LevelData* levelData,int x, int y);
Entity* RaycastFirstEntity(int xOrigin, int yOrigin, Direction direction, LevelData* levelData, bool ignoreWalls = false);
bool IsWalkable(int x, int y, LevelData* levelData);
