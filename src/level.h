#pragma once

#include "arena.h"
#include "entity.h"
#include <cstdint>

using namespace Memory;

struct LevelData {
  int w;
  int h;
  uint8_t* cells;
  const char* levelPath;
  Entity* entityBuffer;
  int entityCount;

  uint8_t GetCell(int x, int y) {
    return cells[y * w + x];
  }

  Entity* GetEntity(int x, int y) {
    for (int i = 0; i < entityCount; i++) {
      if (entityBuffer[i].x == x && entityBuffer[i].y == y) {
        return &entityBuffer[i];
      }
    }
    return nullptr;
  }
  
};

void CreateLevel(Arena* arena, LevelData* levelData, const char* levelName);
void CreateEntities(LevelData* levelData, Arena* arena);
