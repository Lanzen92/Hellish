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
                       float x, float y, float scale, float alpha, bool flipped) {

  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  rect.h = sprite->height * UPSCALE_FACTOR * scale;
  rect.w = sprite->width * UPSCALE_FACTOR * scale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;
  SDL_SetTextureScaleMode(sprite->texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  
  //SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);

  SDL_RenderTextureRotated(renderer, sprite->texture, NULL, &rect, 0.0, NULL,
                           flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE);

}

void RenderSpriteGrid(Sprite* sprite, LevelData* level, SDL_Renderer* renderer,
    const Camera* camera, float x, float y, float scale, float alpha, bool flipped) {
  camera::GridToWorld(&x, &y, level);

  RenderSpriteWorld(sprite, renderer, camera, x, y, scale, alpha, flipped);
}

void RenderEntityOnTile(Sprite* sprite, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera,
    float x, float y, float scale, float alpha, bool flipped) {
  camera::GridToWorld(&x, &y, levelData);
  x += CELL_SIZE_PX / 2.0;
  y += CELL_SIZE_PX / 2.0;
  RenderSpriteWorld(sprite, renderer, camera, x, y, scale, alpha, flipped);
}