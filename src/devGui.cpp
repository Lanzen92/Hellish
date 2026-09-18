#include "devGui.h"
#include "gameState.h"
#include "command.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "SDL3/SDL_render.h"
#include "imgui/imgui_internal.h"
#include "levelEditor.h"
#include <string>

using namespace std;

void DrawImGuiArenaUsage(Arena* arena, std::string nameOfArena) {
  float fraction = (float)arena->used / (float) arena->size;
  string barText = nameOfArena;

  barText += " " + to_string(arena->used);
  barText += " / " + to_string(arena->size);

  ImGui::ProgressBar(fraction, ImVec2(-1,0), barText.c_str());
  
}

void DrawFPS(float dt) {
  ImGui::Text("FPS %0.f", 1 / dt);
}

void DEV::Initialize(SDL_Window* window, SDL_Renderer* renderer) {
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  ImGuiIO& io = ImGui::GetIO();
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  io.DisplaySize = ImVec2((float)w, (float)h);
}

void DEV::ProcessEvents(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);
}

void DEV::Draw(GameData* gameData, SDL_Renderer* renderer) {
  ImGui::Begin("Dev tools");

  DrawImGuiArenaUsage(gameData->arenaImages, "Images");
  DrawImGuiArenaUsage(gameData->arenaLevels, "Levels");
  DrawImGuiArenaUsage(gameData->arenaCommands, "Commands");
  DrawImGuiArenaUsage(gameData->arenaEntities, "Entities");

  if (gameData->editLevel) {
    EDITOR::DrawObjectPanel(&gameData->editorData, gameData->spriteBuffer);
    EDITOR::DrawPreview(&gameData->editorData, &gameData->input, renderer, gameData->GetCurrentLevel(), &gameData->camera, gameData->spriteBuffer);
  }

  DrawFPS(*gameData->dt);

  ImGui::End();
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void DEV::PreDraw(ImGuiContext* savedContext) {
  if (ImGui::GetCurrentContext() == nullptr) {
    ImGui::SetCurrentContext(savedContext);
  } 

  ImGui::NewFrame();
}

void DrawHistory(CommandBuffer* buffer, LevelData* levelData) {
  int sliderPos = buffer->index;

  if (ImGui::SliderInt("History", &sliderPos, 0,  buffer->head)) {
    while (buffer->index > sliderPos) {
      Undo(buffer);
    }
    while (buffer->index < sliderPos) {
      Redo(buffer, levelData);
    }
  }
}

