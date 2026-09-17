
#include "camera.h"
#include "common.h"

bool camera::GetIsPointInsideGrid(float x, float y, const LevelData* level) {
    int xGrid;
    int yGrid;

    WorldToGrid(x, y, &xGrid, &yGrid, level);
    return xGrid >= 0 && yGrid >= 0 && xGrid < level->w && yGrid < level->h;
}

void camera::GridToWorld(float* x, float* y, const LevelData* level) {
  *x *= CELL_SIZE_PX;
  *x += SCREEN_WIDTH / 2.0;
  *x -= level->w * CELL_SIZE_PX / 2.0;

  *y *= CELL_SIZE_PX;
  *y += SCREEN_HEIGHT / 2.0;
  *y -= level->h * CELL_SIZE_PX / 2.0;
}
  
void camera::WorldToGrid(float xWorld, float yWorld, int* x, int* y, const LevelData* level) {
  *x = xWorld;
  *y = yWorld;
  *x += level->w * CELL_SIZE_PX / 2.0;
  *x -= SCREEN_WIDTH / 2.0;
  *x /= CELL_SIZE_PX;
  *y += level->h * CELL_SIZE_PX / 2.0;
  *y -= SCREEN_HEIGHT / 2.0;
  *y /= CELL_SIZE_PX;
}


