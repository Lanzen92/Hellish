#include <cmath>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

#include "camera.h"
#include "common.h"
#include "spriteLibrary.h"
#include "rendering.h"
#include "button.h"
#include "fontLibrary.h"
#include "gameState.h"

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
  SDL_Texture* texture = button->sprite->texture;
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
  
  uint8_t colorOverlay = isSelected ? 255 : 230;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, colorOverlay, colorOverlay, colorOverlay);
  SDL_RenderTexture(renderer, button->sprite->texture, NULL, &button->rect);
  
  if(!IsStringEmpty(button->text)){
    float glyph_height = button->font->glyphs['H'].atlasPosition.h / 2.0;
    RenderText(button->font, button->text, renderer, nullptr, button->rect.x + (button->rect.w / 2.0), button->rect.y + (button->rect.h / 2.0) - glyph_height, Alignment::Centered);
  }
}

void RenderButtonDynamic(Button* button, bool isSelected, SDL_Renderer* renderer) {
  assert(button->sprite->spriteCountX == 3);
  assert(button->sprite->spriteCountY == 3);
  
  uint8_t colorOverlay = isSelected ? 255: 230;
  SDL_Texture* texture = button->sprite->texture;
  SDL_FRect rect = button->rect;
  
  float partW = texture->w / 3.0;
  float partH = texture->h / 3.0;
  float verticalCenterHeight = rect.h - (partH * 2);
  float horizontalCenterWidth = rect.w - (partW * 2);
  
  float rightX = rect.x + rect.w - partW;
  float bottomY = rect.y + rect.h - partH;
  float centerY = rect.y + partH;
  float centerX = rect.x + partW;
  
  SDL_FRect topLeftdst = {  rect.x, rect.y, partW, partH  };
  SDL_FRect topRightdst = { rightX, rect.y, partW, partH };
  SDL_FRect topCenterdst = {  centerX, rect.y, horizontalCenterWidth, partH };
  SDL_FRect bottomLeftdst = { rect.x, bottomY, partW, partH };
  SDL_FRect bottomRightdst = {  rightX, bottomY, partW, partH };
  SDL_FRect bottomCenterdst = { centerX, bottomY, horizontalCenterWidth, partH  };
  SDL_FRect centerLeftdst = { rect.x, centerY, partW, verticalCenterHeight };
  SDL_FRect centerRightdst = {  rightX, centerY, partW, verticalCenterHeight };
  SDL_FRect centerdst = { centerX, centerY, horizontalCenterWidth, verticalCenterHeight };
  
  SDL_FRect topLeftsrc = {0, 0, partW, partH};
  SDL_FRect topRightsrc = {partW * 2, 0, partW, partH};
  SDL_FRect topCentersrc = {partW * 1, 0, partW, partH};
  SDL_FRect bottomLeftsrc = {0, partH * 2, partW, partH};
  SDL_FRect bottomRightsrc = {partW * 2, partH * 2, partW, partH};
  SDL_FRect bottomCentersrc = {partW * 1, partH * 2, partW, partH};
  SDL_FRect centerLeftsrc = {0, partH * 1, partW, partH};
  SDL_FRect centerRightsrc = {partW * 2, partH * 1, partW, partH};
  SDL_FRect centersrc = {partW * 1, partH * 1, partW, partH};
  
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, colorOverlay, colorOverlay, colorOverlay);
  
  SDL_RenderTexture(renderer, texture, &topLeftsrc, &topLeftdst );
  SDL_RenderTexture(renderer, texture, &topCentersrc, &topCenterdst );
  SDL_RenderTexture(renderer, texture, &bottomCentersrc, &bottomCenterdst );
  SDL_RenderTexture(renderer, texture, &centerLeftsrc, &centerLeftdst );
  SDL_RenderTexture(renderer, texture, &centerRightsrc, &centerRightdst );
  SDL_RenderTexture(renderer, texture, &bottomLeftsrc, &bottomLeftdst );
  SDL_RenderTexture(renderer, texture, &topRightsrc, &topRightdst );
  SDL_RenderTexture(renderer, texture, &bottomRightsrc, &bottomRightdst );
  SDL_RenderTexture(renderer, texture, &centersrc, &centerdst );
  
  if(!IsStringEmpty(button->text)) {
    float glyphHeight = button->font->glyphs['H'].atlasPosition.h / 2.0;
    RenderText(button->font, button->text, renderer, nullptr, rect.x + (rect.w / 2.0), rect.y + (rect.h / 2.0) - glyphHeight, Alignment::Centered);
  }
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
}