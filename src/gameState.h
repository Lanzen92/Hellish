#pragma once

#include <stdint.h>

#include "imgui/imgui.h"

#include "arena.h"
#include "command.h"
#include "levelEditor.h"
#include "spriteLibrary.h"
#include "level.h"
#include "input.h"
#include "camera.h"

//enum class GAME_STATES { PLAY, BUILD };

struct GameData {

  const float* dt;

  bool editLevel;
  Editor editorData;
    
  ImGuiContext* imGuiContext;

  Camera camera;
    
  Position* inputBuffer;
  int inputBufferCapacity;
  int inputBufferWriteCount;
  int inputBufferReadCount;
  
  Sprite* spriteBuffer;
  Input input;

  CommandBuffer* commandBuffer;

  Memory::Arena* arenaMain;
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;
  Memory::Arena* arenaCommands;
  Memory::Arena* arenaScratch;
  Memory::Arena* arenaInputs;

  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  float* fpsBuffer;
  int fpsBufferCount;
  int fpsBufferIndex;

  LevelData* GetCurrentLevel() {
    return &levels[currentLevelIndex];
  }
};
