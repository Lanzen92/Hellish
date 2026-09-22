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

enum class SCENE_TYPES : uint8_t {
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS,
};

struct Gameplay {
  
  CommandBuffer* commandBuffer;
  
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  
  Position* inputBuffer;
  int inputBufferCapacity;
  int inputBufferWriteCount;
  int inputBufferReadCount;
  
  bool initialized;
};

struct MainMenu {
  
};

struct Titlescreen {
  
};

struct Credits {
  
};

struct Transition {
  enum States {
    Inactive,
    FadeTo,
    FadeFrom
  };
  
  States state;
  float fadeTimeElapsed;
  float fadeTimeDuration = 1.0f;
};

struct Scenes {
  Gameplay gameplay;
  MainMenu mainMenu;
  Titlescreen titleScreen;
  Credits credits;
};

struct EditorData {
  float* fpsBuffer;
  int fpsBufferCount;
  int fpsBufferIndex;
  
  bool editLevel;
  Editor editor;
};

struct GameData {

  SCENE_TYPES sceneCurrent;
  SCENE_TYPES scenePrevious;
  Scenes scenes;
  
  Transition transition;
  EditorData editorData;
  ImGuiContext* imGuiContext;
  
  Memory::Arena* arenaMain;
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;
  Memory::Arena* arenaCommands;
  Memory::Arena* arenaScratch;
  Memory::Arena* arenaInputs;
  
  Camera camera;
  Sprite* spriteBuffer;
  Input input;
  
  const float* dt;
};

inline LevelData* GetCurrentLevel(Gameplay* gameplay) {
  return &gameplay->levels[gameplay->currentLevelIndex];
}
