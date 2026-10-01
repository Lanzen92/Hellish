#include "camera.h"
#include "common.h"

bool CAMERA::GetIsPointInsideGrid(float x, float y, const LevelData* level) {
    int xGrid;
    int yGrid;

    WorldToGrid(x, y, &xGrid, &yGrid, level);
    return xGrid >= 0 && yGrid >= 0 && xGrid < level->w && yGrid < level->h;
}

void CAMERA::GridToWorld(float* x, float* y, const LevelData* level) {
  *x *= TILE_SIZE_PX_SCALED;
  *x += TILE_SIZE_PX_SCALED;
  *x += SCREEN_WIDTH / 2.0;
  *x -= level->w * TILE_SIZE_PX_SCALED / 2.0;

  *y *= TILE_SIZE_PX_SCALED;
  *y += TILE_SIZE_PX_SCALED;
  *y += SCREEN_HEIGHT / 2.0;
  *y -= level->h * TILE_SIZE_PX_SCALED / 2.0;
}
  
void CAMERA::WorldToGrid(float xWorld, float yWorld, int* x, int* y, const LevelData* level) {
  *x = xWorld;
  *y = yWorld;

  *x += level->w * TILE_SIZE_PX_SCALED / 2.0;
  *x -= SCREEN_WIDTH / 2.0;
  *x /= TILE_SIZE_PX_SCALED;

  *y += level->h * TILE_SIZE_PX_SCALED / 2.0;
  *y -= SCREEN_HEIGHT / 2.0;
  *y /= TILE_SIZE_PX_SCALED;
}


