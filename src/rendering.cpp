#include <cmath>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "camera.h"
#include "common.h"
#include "spriteLibrary.h"
#include "rendering.h"

#include "fontLibrary.h"

static const char STOP_CHAR = '\0';

void RenderSpriteWorld(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera* camera, 
                       float x, float y, float scale, float alpha, bool flipped) {
  
  int frame = spriteRenderInfo.frame;
  Sprite* sprite = spriteRenderInfo.sprite;
  SDL_FRect tilesetRect;
  
  if(GetSpriteCount(sprite) > 1) {
    int width = sprite->width / sprite->spriteCountX;
    int height = sprite->height / sprite->spriteCountY;
    tilesetRect.w = width;
    tilesetRect.h = height;
    // tilesetRect.x = (frame % sprite->spriteCountX) * width;
    // tilesetRect.y = (frame / sprite->spriteCountX) * height;
    Expand1DTo2D(frame, sprite->spriteCountX, &tilesetRect.x, &tilesetRect.y);
    tilesetRect.x *= width;
    tilesetRect.y *= height;
  }
  else {
    tilesetRect.w = sprite->width;
    tilesetRect.h = sprite->height;
    tilesetRect.x = 0;
    tilesetRect.y = 0; 
  }
  
  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  float final_scale = UPSCALE_FACTOR * scale; 
  rect.h = tilesetRect.h * final_scale;
  rect.w = tilesetRect.w * final_scale;
  rect.x -= sprite->pivotX * final_scale;
  rect.y -= sprite->pivotY * final_scale;
  
  if (camera != NULL) {
    rect.x -= camera->cameraX;
    rect.y -= camera->cameraY;
  }
  
  SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  SDL_FlipMode flip = (flipped || spriteRenderInfo.flippedX) ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
  
  SDL_RenderTextureRotated(renderer, sprite->texture, &tilesetRect, &rect, 0, 0, flip);
}

void RenderTile(Sprite* tileset, int cellId, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera,
                float x, float y, float scale, float alpha) {
  CAMERA::GridToWorld(&x, &y, levelData);
  RenderSpriteWorld({cellId, tileset}, renderer, camera, x, y, scale, alpha, false);
}

void RenderSpriteOnTile(SpriteRenderInfo spriteInfo, LevelData* levelData, SDL_Renderer* renderer, const Camera* camera, 
                        float x, float y, float scale, float alpha, bool flipped){
  CAMERA::GridToWorld(&x, &y, levelData);
  x += TILE_SIZE_PX_SCALED / 2.0;
  y += TILE_SIZE_PX_SCALED / 2.0;
  RenderSpriteWorld(spriteInfo,renderer, camera, x, y, scale, alpha, flipped);
}

void RenderButton(Button* button, bool isSelected, SDL_Renderer* renderer) {
  SDL_Texture* texture = button->texture;
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
  
  uint8_t colorOverlay = isSelected ? 255 : 230;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, colorOverlay, colorOverlay, colorOverlay);
  SDL_RenderTexture(renderer, button->texture, NULL, &button->rect);
  
}

//Renders background that fills the window.
void RenderBackground(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, float alpha, bool flipped) {
  Sprite* sprite = spriteRenderInfo.sprite;
  if (!sprite) return; 
  
  float scaleX = (float)SCREEN_WIDTH / ((float)sprite->width * UPSCALE_FACTOR);
  float scaleY = (float)SCREEN_HEIGHT / ((float)sprite->height * UPSCALE_FACTOR);
  float scale = std::max(scaleX, scaleY); 
  
  float finalWidth = sprite->width * (UPSCALE_FACTOR * scale);
  float finalHeight = sprite->height * (UPSCALE_FACTOR * scale);
  
  SDL_FRect rect;
  rect.x = (SCREEN_WIDTH - finalWidth) / 2.0f;
  rect.y = (SCREEN_HEIGHT - finalHeight) / 2.0f;
  rect.w = finalWidth;
  rect.h = finalHeight;
  
  SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  SDL_FlipMode flip = (flipped || spriteRenderInfo.flippedX) ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
  SDL_RenderTextureRotated(renderer, sprite->texture, nullptr, &rect, 0, 0, flip);
}

void RenderText(FontAtlas* atlas, const char* text, SDL_Renderer* renderer, Camera* camera, const float x, const float y, Alignment mode, Type type) {
  assert(atlas->atlasTexture != nullptr);
  
  float scale = 1.0f;
  if (type == Type::Header) scale = 1.3f;
  
  float drawPositionX = x;
  float drawPositionY = y;
  if(camera != nullptr){
    drawPositionX -= camera->cameraX;
    drawPositionY -= camera->cameraY;
  }
  
  if(mode == Alignment::Centered){
    float totalWidth = 0;
    for (int i = 0; text[i] != STOP_CHAR; i++) {
      totalWidth += atlas->glyphs[text[i]].atlasPosition.w * scale;
    }
    drawPositionX -= totalWidth / 2.0;
  }
  
  //For dynamic scaling of the font.
  for (int i = 0; text[i] != '\0'; i++) {
    Glyph glyph = atlas->glyphs[text[i]];
    
    SDL_FRect renderRectangle = {
      drawPositionX, 
      drawPositionY, 
      glyph.atlasPosition.w * scale, 
      glyph.atlasPosition.h * scale
  };
    
    SDL_RenderTexture(renderer, atlas->atlasTexture, &glyph.atlasPosition, &renderRectangle);
    drawPositionX += glyph.atlasPosition.w * scale; 
  }

  
  // for (int i = 0; text[i] != STOP_CHAR; i++) {
  //   Glyph glyph = atlas->glyphs[text[i]];
  //   SDL_FRect renderRectangle = {drawPositionX, drawPositionY, glyph.atlasPosition.w, glyph.atlasPosition.h};
  //   SDL_RenderTexture(renderer, atlas->atlasTexture, &glyph.atlasPosition, &renderRectangle);
  //   drawPositionX += glyph.atlasPosition.w;
  // }
}