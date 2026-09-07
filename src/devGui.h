#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "imgui/imgui_impl_sdl3.h"
#include "gameState.h"

namespace DEV {
  void Initialize(SDL_Window* window, SDL_Renderer* renderer);
  void ProcessEvents(SDL_Event* event);
  void PreDraw(ImGuiContext* savedContext);
  void Draw(GameData* gameData, SDL_Renderer* renderer);
}
