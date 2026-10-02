#include <cassert>

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL_TTF/SDL_ttf.h"

#include "fontLibrary.h"

namespace AssetManagement{
  void LoadFont(SDL_Renderer* renderer, const char* fontPath, FontAtlas* fontAtlas, float ptSize){
    
    TTF_Font* font = TTF_OpenFont(fontPath, ptSize);
    assert(font != nullptr);
    SDL_Color white = {255,255,255,255};
    
    int atlasSize = 1024;
    SDL_Surface* atlasSurface;
    atlasSurface = SDL_CreateSurface(atlasSize, atlasSize, SDL_PIXELFORMAT_RGBA32);
    
    int drawPointX = 0;
    int drawPointY = 0;
    int tallestGlyphInRow = 0;
    
    int FIRST_RELEVANT_GLYPH = 32;
    
    for (int i = FIRST_RELEVANT_GLYPH; i < FontAtlas::GLYPH_COUNT; i++) {
      SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(font, i, white);
      if(glyphSurface == nullptr){
        continue;
      }
      
      if(drawPointX + glyphSurface->w > atlasSize){
        drawPointX = 0;
        drawPointY += tallestGlyphInRow;
        tallestGlyphInRow = 0;
      }
      
      if(tallestGlyphInRow < glyphSurface->h){
        tallestGlyphInRow = glyphSurface->h;
      }
      
      SDL_Rect glyph_position = {drawPointX, drawPointY, glyphSurface->w,glyphSurface->h};
      SDL_BlitSurface(glyphSurface, NULL, atlasSurface, &glyph_position);
      fontAtlas->glyphs[i].atlasPosition = {(float)glyph_position.x,
      (float)glyph_position.y,
      (float)glyph_position.w,
      (float)glyph_position.h};
      drawPointX += glyphSurface->w;
      SDL_DestroySurface(glyphSurface);
    }
    
    fontAtlas->atlasTexture = SDL_CreateTextureFromSurface(renderer, atlasSurface);
    SDL_DestroySurface(atlasSurface);
    TTF_CloseFont(font);
  }
}