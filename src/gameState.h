#pragma once

#include "SDL3/SDL_rect.h"
#include "image.h"
#include "level.h"

struct GameData {
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;

  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;

  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

};
