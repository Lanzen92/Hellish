#pragma once

#include "command.h"
#include "image.h"
#include "level.h"
#include "imgui/imgui.h"
#include <stdint.h>

struct GameData {

  const float* dt;
  uint32_t commandTimestamp;
  
  ImGuiContext* imGuiContext;
  
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;
  Image* box;
  
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;
  Memory::Arena* arenaCommands;

  bool* keysPrevious;
  CommandBuffer* commandBuffer;
  
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  LevelData* GetCurrentLevel() {
    return &levels[currentLevelIndex];
  }
};
