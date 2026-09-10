#pragma once

#include "level.h"

struct Camera {
  float cameraX;
  float cameraY;
};

namespace camera {
  void GridToWorld(float* x, float* y, const LevelData* level);
  void WorldToGrid(float xWorld, float yWorld, int* x, int* y, const LevelData* level);
  bool GetIsPointInsideGrid(float x, float y, const LevelData* level);
}
