#include "SDL3/SDL_render.h"
#include "camera.h"
#include "common.h"
#include "rendering.h"

void RenderSpriteWorld(Image* sprite, SDL_Renderer* renderer, const Camera* camera,
                       float x, float y, float scale) {

  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  rect.h = sprite->height * UPSCALE_FACTOR * scale;
  rect.w = sprite->width * UPSCALE_FACTOR * scale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}

void RenderSpriteGrid(Image* sprite, LevelData* level, SDL_Renderer* renderer,
                      const Camera* camera, float x, float y, float scale) {
  camera::GridToWorld(&x, &y, level);
  RenderSpriteWorld(sprite, renderer, camera, x, y, scale);
}
