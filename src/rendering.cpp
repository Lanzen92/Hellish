#include <cmath>
#include <algorithm>
#include "SDL3/SDL_render.h"
#include "camera.h"
#include "common.h"
#include "spriteLibrary.h"
#include "rendering.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"


void RenderSpriteWorld(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera,
                       float x, float y, float scale, float alpha) {

  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  rect.h = sprite->height * UPSCALE_FACTOR * scale;
  rect.w = sprite->width * UPSCALE_FACTOR * scale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  SDL_SetTextureAlphaMod(sprite->texture, alpha);
  SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}

void RenderSpriteGrid(Sprite* sprite, LevelData* level, SDL_Renderer* renderer,
                      const Camera* camera, float x, float y, float scale, float alpha) {
  camera::GridToWorld(&x, &y, level);

  RenderSpriteWorld(sprite, renderer, camera, x, y, scale, alpha);
}
