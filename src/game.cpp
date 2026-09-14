#include "game.h"
#include "SDL3/SDL_scancode.h"
#include "command.h"
#include "entity.h"
#include "imgui/imgui.h"
#include "input.h"
#include "levelEditor.h"
#include "levelRenderer.h"
#include "level.h" 
#include "devGui.h"
#include "common.h"
#include "spriteLibrary.h"
#include <cstdint>

  bool TryMove(Entity* mover, LevelData* levelData, CommandBuffer* commandBuffer, int xDir, int yDir, uint32_t timestamp) {
    
    if (mover->HasBehaviour(CAN_MOVE) == false) {
      return false;
    }
  
    int testX = mover->x + xDir;
    int testY = mover->y + yDir;

    Entity* stepIntoEntity = levelData->GetEntity(testX, testY);
    ID StepIntoTileId = (ID)levelData->GetCell(testX, testY);

    if (stepIntoEntity == nullptr) {
      if (StepIntoTileId == ID::GROUND) {

        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;

        Push(commandBuffer, mv, timestamp);
        return true;
        
      }
      
      return false;
    }

    if (stepIntoEntity->HasBehaviour(CAN_MOVE)) {
      if(TryMove(stepIntoEntity, levelData, commandBuffer, xDir, yDir, timestamp)) {
        
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;

        Push(commandBuffer, mv, timestamp);
        return true;
        
      }
    }
    
    return false;
  }
extern "C" {
  void Initialize(GameData* gameData, SDL_Window* window,  SDL_Renderer* renderer) {

    DEV::Initialize(window, renderer);
    gameData->imGuiContext = ImGui::GetCurrentContext();

    AssetManagement::LoadAllSprites(gameData->spriteBuffer, renderer);

    gameData->currentLevelIndex = 1;
    CreateLevel(gameData->arenaLevels, &gameData->levels[0], "assets/maps/testmap.tmj");
    CreateLevel(gameData->arenaLevels, &gameData->levels[1], "assets/maps/testmap_box.tmj");
    CreateEntities(&gameData->levels[gameData->currentLevelIndex], gameData->arenaEntities);
  }
  
  bool HandleEvents(GameData* data, SDL_Event event) {
    DEV::ProcessEvents(&event);

    if (event.type != SDL_EVENT_KEY_DOWN) {
      return true;
    }
    if (event.key.key == SDLK_ESCAPE) {
      return false;
    }

    return true;
  }

  void Update(GameData* gameData, float dt) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (KeyPressed(&gameData->input, SDL_SCANCODE_Z) ||
        KeyHeldForTime(&gameData->input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME)) {
      ResetKeyHeldTime(&gameData->input, SDL_SCANCODE_Z);
          
      if (KeyHeld(&gameData->input, SDL_SCANCODE_LSHIFT)) {
        Redo(gameData->commandBuffer);
      }
      else {
        Undo(gameData->commandBuffer);
      }
    }

    if (KeyPressed(&gameData->input, SDL_SCANCODE_F2)) {
      gameData->editLevel = !gameData->editLevel;
    }

    if (gameData->editLevel) {
      EDITOR::Update(&gameData->editorData, &gameData->input, gameData->GetCurrentLevel());
    }
    
    if (KeyPressed(&gameData->input, SDL_SCANCODE_RIGHT) ||
        KeyHeldForTime(&gameData->input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)) {

      ResetKeyHeldTime(&gameData->input, SDL_SCANCODE_RIGHT);
      gameData->inputBuffer[gameData->inputBufferWriteCount++ % gameData->inputBufferCapacity] = {1, 0};
    }  
    else if (KeyPressed(&gameData->input, SDL_SCANCODE_LEFT) ||
             KeyHeldForTime(&gameData->input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15))  {

      ResetKeyHeldTime(&gameData->input, SDL_SCANCODE_LEFT);
      gameData->inputBuffer[gameData->inputBufferWriteCount++ % gameData->inputBufferCapacity] = {-1, 0};
    }
    else if (KeyPressed(&gameData->input, SDL_SCANCODE_UP) ||
             KeyHeldForTime(&gameData->input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15))  {

      ResetKeyHeldTime(&gameData->input, SDL_SCANCODE_UP);
      gameData->inputBuffer[gameData->inputBufferWriteCount++ % gameData->inputBufferCapacity] = {0, -1};
    }
    else if (KeyPressed(&gameData->input, SDL_SCANCODE_DOWN) ||
             KeyHeldForTime(&gameData->input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15))  {

      ResetKeyHeldTime(&gameData->input, SDL_SCANCODE_DOWN);
      gameData->inputBuffer[gameData->inputBufferWriteCount++ % gameData->inputBufferCapacity] = {0, 1};
    }

    bool areEntitiesMoving = false;
    for  (int i = 0; i < gameData->GetCurrentLevel()->entityCount; i++) {
      Entity* entity = &gameData->GetCurrentLevel()->entityBuffer[i];

      if (entity->HasBehaviour(CAN_MOVE) && IsMoving(entity)) {
        entity->progress01 += MOVE_SPEED * dt;

        if (entity->progress01 >= 1) {
          entity->progress01 = 0;
          entity->xPrev = entity->x;
          entity->yPrev = entity->y;
        }

        if (IsMoving(entity)) {
          areEntitiesMoving = true;
        }
      }
    }

    if (!areEntitiesMoving) {
      if(gameData->inputBufferReadCount == gameData->inputBufferWriteCount) {
        return;
      }

      gameData->commandTimestamp += 1;

      for (int i = 0; i < gameData->GetCurrentLevel()->entityCount; i++) {
        Entity* entity = &gameData->GetCurrentLevel()->entityBuffer[i];

        if (entity->HasBehaviour((Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))) {
          int xDir = gameData->inputBuffer[gameData->inputBufferReadCount % gameData->inputBufferCapacity].x;
          int yDir = gameData->inputBuffer[gameData->inputBufferReadCount % gameData->inputBufferCapacity].y;

          TryMove(entity, gameData->GetCurrentLevel(), gameData->commandBuffer, xDir, yDir, gameData->commandTimestamp);

        } 
      }

      gameData->inputBufferReadCount++;
    }
    
  }
  
  
  void Draw(GameData* gameData, SDL_Renderer* renderer) {
    
    DEV::PreDraw(gameData->imGuiContext);
    
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);

    RenderLevel(gameData, renderer);
    RenderEntities(gameData, renderer);

    DEV::Draw(gameData, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
  }
}
