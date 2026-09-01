// #include <windows.h>
// #include <fileapi.h>
 #include <cstdio>

// #include "SDL3/SDL_keyboard.h"
// #include "SDL3/SDL_render.h"

#include "game.h"
#include "rendering.h"
#include "levelRenderer.h"
#include "level.h" 
//#include "gameState.h"

extern "C" {
  void Initialize(GameData* gameData, SDL_Renderer* renderer) {
    gameData->ground = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "ground.png");
    gameData->wall = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "wall.png");
    gameData->player = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "player.png");

    gameData->currentLevelIndex = 0;
    CreateLevel(gameData->arenaLevels, &gameData->levels[0], "assets/maps/testmap.tmj");
    CreateEntities(&gameData->levels[gameData->currentLevelIndex], gameData->arenaEntities);
  }
  
  bool HandleEvents(GameData* data, SDL_Event event) {
    if (event.type != SDL_EVENT_KEY_DOWN) {
      return true;
    }
    if (event.key.key == SDLK_ESCAPE) {
      return false;
    }

    return true;
  }

  void Update(GameData* data, float dt) {
    const bool* keys = SDL_GetKeyboardState(NULL);

    // if(keys[SDL_SCANCODE_RIGHT]) {
    //   data->rect.x += data->moveSpeed * dt;
    // }

    // if (keys[SDL_SCANCODE_LEFT]) {
    //   data->rect.x -= data->moveSpeed * dt;
    // }

    // if (keys[SDL_SCANCODE_UP]) {
    //   data->rect.y -= data->moveSpeed * dt;
    // }

    // if (keys[SDL_SCANCODE_DOWN]) {
    //   data->rect.y += data->moveSpeed * dt;
    // }
  }
     
  void Draw(GameData* gameData, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);

    RenderLevel(gameData, renderer);
    RenderEntities(gameData, renderer);

    // RenderSprite(data->fallback, renderer, static_cast<int>(data->rect.x), static_cast<int>(data->rect.y));

    // RenderSprite(data->fallback, renderer, (int)&data->rect.x,(int)&data->rect.y);
    // SDL_RenderTexture(renderer, data->fallback->texture, NULL, &data->rect);

    // SDL_SetRenderDrawColor(renderer, 150, 0, 30, 255);
    // SDL_RenderFillRect(renderer, &data->rect);

    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
  }
}
