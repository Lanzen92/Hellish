#include "game.h"
#include "SDL3/SDL_scancode.h"
#include "command.h"
#include "entity.h"
#include "imgui/imgui.h"
#include "levelRenderer.h"
#include "level.h" 
#include "devGui.h"
#include <cstdint>

bool KeyPressed(SDL_Scancode key, const bool* current, const bool* previous) {
  if (previous == nullptr) {
    return current[key];
  }
  return current[key] && !previous[key];
}

bool KeyHeld(SDL_Scancode key, const bool* current, const bool* previous) {
  if (previous == nullptr) {
    return false;
  }
  return current[key] && previous[key];
}

bool KeyReleased(SDL_Scancode key, const bool* current, const bool* previous) {
  if (previous == nullptr) {
    return false;
  }
  return !current[key] && previous[key];
}

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
    gameData->ground = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "ground.png");
    gameData->wall = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "wall.png");
    gameData->player = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "player.png");
    gameData->box = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "box.png");
    gameData->fallback = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "fallback.png");

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

    gameData->commandTimestamp++;

    for (int i = 0; i < gameData->GetCurrentLevel()->entityCount; i++) {
      Entity* entity = &gameData->GetCurrentLevel()->entityBuffer[i];
    
      if (entity->HasBehaviour((Behaviour)(Behaviour::RESPOND_TO_INPUT | Behaviour::CAN_MOVE))) {
        int xChange = 0;
        int yChange = 0;

        if (KeyPressed(SDL_SCANCODE_RIGHT, keys, gameData->keysPrevious)) {
          xChange = 1;
        }     
        else if (KeyPressed(SDL_SCANCODE_LEFT, keys, gameData->keysPrevious)) {
          xChange = -1;
        }     
        else if (KeyPressed(SDL_SCANCODE_UP, keys, gameData->keysPrevious)) {
          yChange = -1;
        }     
        else if (KeyPressed(SDL_SCANCODE_DOWN, keys, gameData->keysPrevious)) {
          yChange = 1;
        }
        else if (KeyPressed(SDL_SCANCODE_Z, keys, gameData->keysPrevious)) {
          if (KeyHeld(SDL_SCANCODE_LSHIFT, keys, gameData->keysPrevious)) {
            Redo(gameData->commandBuffer);
          }
          else {
            Undo(gameData->commandBuffer);
          }
        }
        

        if (xChange != 0 || yChange != 0) {
          TryMove(entity, gameData->GetCurrentLevel(), gameData->commandBuffer,  xChange, yChange, gameData->commandTimestamp);

        }
      }     
    }
    
     memcpy((void*)gameData->keysPrevious, keys, SDL_SCANCODE_COUNT * sizeof(bool));   
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
