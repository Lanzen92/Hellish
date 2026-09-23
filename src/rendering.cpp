#include <cmath>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "camera.h"
#include "common.h"
#include "spriteLibrary.h"
#include "rendering.h"

void RenderSpriteWorld(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera,
                       float x, float y, float scale, float alpha, bool flipped) {

  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  float finalScale = UPSCALE_FACTOR * scale;
  rect.h = sprite->height * finalScale;
  rect.w = sprite->width * finalScale;
  rect.x -= sprite->pivotX * finalScale;
  rect.y -= sprite->pivotY * finalScale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  SDL_SetTextureScaleMode(sprite->texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  
  SDL_RenderTextureRotated(renderer, sprite->texture, NULL, &rect, 0.0, NULL,
                           flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE);

}

void RenderTileWorld(Sprite* tilesetAtlasSprite, int cellId, LevelData* leveldata, SDL_Renderer* renderer, 
                     const Camera* camera, float x, float y, float scale, float alpha) {
  CAMERA::GridToWorld(&x, &y, leveldata);
  
  SDL_FRect tilesetRect;
  tilesetRect.w = TILE_SIZE_PX_RAW;
  tilesetRect.h = TILE_SIZE_PX_RAW;
  tilesetRect.x = (cellId % tilesetAtlasSprite->tilesetCellCountX) * TILE_SIZE_PX_RAW;
  tilesetRect.y = (cellId / tilesetAtlasSprite->tilesetCellCountY) * TILE_SIZE_PX_RAW;
  
  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  float finalScale = scale * UPSCALE_FACTOR;
  rect.h = TILE_SIZE_PX_RAW * finalScale;
  rect.w = TILE_SIZE_PX_RAW * finalScale;
  rect.x -= tilesetAtlasSprite->pivotX * finalScale;
  rect.y -= tilesetAtlasSprite->pivotY * finalScale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  SDL_SetTextureScaleMode(tilesetAtlasSprite->texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(tilesetAtlasSprite->texture, alpha);
  SDL_RenderTexture(renderer, tilesetAtlasSprite->texture, &tilesetRect, &rect);
}

void RenderEntityOnTile(Sprite* sprite, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera,
                        float x, float y, float scale, float alpha, bool flipped) {
  CAMERA::GridToWorld(&x, &y, levelData);
  x += TILE_SIZE_PX_SCALED / 2.0;
  y += TILE_SIZE_PX_SCALED / 2.0;
  RenderSpriteWorld(sprite, renderer, camera, x, y, scale, alpha, flipped);
}