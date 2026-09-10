#pragma once

#include <stdint.h>

#include "imgui/imgui.h"

#include "arena.h"
#include "command.h"
#include "image.h"
#include "level.h"
#include "input.h"
#include "camera.h"

struct GameData {

  const float* dt;
  uint32_t commandTimestamp;
  
  ImGuiContext* imGuiContext;

  Camera camera;
    
  Position* inputBuffer;
  int inputBufferCapacity;
  int inputBufferWriteCount;
  int inputBufferReadCount;
  
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;
  Image* box;
  
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;
  Memory::Arena* arenaCommands;

  Input input;
  Memory::Arena* arenaInputs;

  CommandBuffer* commandBuffer;
  
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  LevelData* GetCurrentLevel() {
    return &levels[currentLevelIndex];
  }
};
