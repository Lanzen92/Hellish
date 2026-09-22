#include <algorithm>
#include <complex>

#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_log.h"
#include "imgui/imgui.h"

#include "game.h"
#include "command.h"
#include "common.h"
#include "devGui.h"
#include "entity.h"
#include "input.h"
#include "level.h" 
#include "levelEditor.h"
#include "levelRenderer.h"
#include "rendering.h"
#include "spriteLibrary.h"

extern "C" {

  void StartLevel(Gameplay* gameplay, Arena* arenaCommands, Arena* arenaEntities) {
    Reset(arenaCommands);
    CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], arenaEntities);
  }

  bool TryMove(Entity* mover, LevelData* levelData, CommandBuffer* commandBuffer, int xDir, int yDir, int strength) {

    if (strength < 0) {
      return false;
    }

    if (HasBehaviour(mover, CAN_MOVE) == false) {
      return false;
    }

    int testX = mover->x + xDir;
    int testY = mover->y + yDir;

    Entity* stepIntoEntity = GetEntity(levelData, testX, testY);
    ID StepIntoTileId = (ID)GetCellID(levelData, testX, testY);

    if (stepIntoEntity == nullptr) {
      if (StepIntoTileId == ID::GROUND) {
        MoveCommand mv(mover, xDir, yDir);
        Push(commandBuffer, mv, levelData);
        return true;
      }

      return false;
    }

    if (HasBehaviour(stepIntoEntity, CAN_MOVE) &&
        !HasBehaviour(stepIntoEntity, UNPUSHABLE)) {
      if (TryMove(stepIntoEntity, levelData, commandBuffer, xDir, yDir,
                  --strength)) {
        MoveCommand mv(mover, xDir, yDir);
        AddBehaviour(mover, Behaviour::IS_PUSHING);
        Push(commandBuffer, mv, levelData);
        return true;
      }
    }

    return false;
  }

  void InitializeGame(Gameplay* gameplay, Arena* arenaLevels) {
    assert(gameplay->initialized == false);
        
    gameplay->currentLevelIndex = 1;
    CreateLevel(arenaLevels, &gameplay->levels[0], "assets/maps/testmap.tmj");
    CreateLevel(arenaLevels, &gameplay->levels[1], "assets/maps/testmap_box.tmj");
    //CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], gameplay->arenaEntities);
    gameplay->initialized = true;
  }

  void ChangeScene(GameData* gameData, SCENE_TYPES newScene) {
    assert (newScene != gameData->sceneCurrent);
      
    gameData->scenePrevious = gameData->sceneCurrent;
    gameData->sceneCurrent = newScene;
    gameData->transition.state = gameData->scenePrevious == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    gameData->transition.fadeTimeElapsed = 0;
      
    switch (gameData->sceneCurrent) {
    case SCENE_TYPES::TITLESCREEN: {
      SDL_Log("Change to TitleScreen");
      gameData->transition.fadeTimeDuration = 1;
      break;
    }
    case SCENE_TYPES::MAINMENU: {
      SDL_Log("Changed to MainMenu");
      break;
    }
    case SCENE_TYPES::GAME: {
      SDL_Log("Changed to Game");
      gameData->transition.fadeTimeDuration = 0.5f;
      Gameplay* gameplay = &gameData->scenes.gameplay;
      assert(gameplay->initialized);
      StartLevel(gameplay, gameData->arenaCommands, gameData->arenaEntities);
      break;
    }
    case SCENE_TYPES::CREDITS: {
      SDL_Log("Changed to Credits");
      break;
    }
    case SCENE_TYPES::NONE: {
      
      assert(false);
      break;
      }
    }
  }

  void DrawScene(GameData* gameData, SCENE_TYPES scene, SDL_Renderer* renderer) {
    switch(scene) {
      case SCENE_TYPES::TITLESCREEN: {
        Sprite* background = GetSprite(SPRITE_ID::TitleScreenBackground, gameData->spriteBuffer);
        RenderSpriteWorld(background, renderer, &gameData->camera, 0, 0);
        break;
      }
      case SCENE_TYPES::MAINMENU:
      case SCENE_TYPES::GAME: 
        RenderLevel(gameData, renderer);
        RenderEntities(gameData, renderer);
        break;

      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
        break;
      }
  }

  void UpdateTitleScreen(Titlescreen* titlescreen, const float dt) {
    
  }

  void UpdateGame(Gameplay* gameplay, Input* input, const float dt) {
    
    float undoSpeedUp = std::lerp(1.0, 0.15, (gameplay->commandBuffer->head - gameplay->commandBuffer->index) * (1.0/30.0));
    undoSpeedUp = std::max<double>(undoSpeedUp, 0.15);

    if (KeyPressed(input, SDL_SCANCODE_Z) ||
        KeyHeldForTime(input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME * undoSpeedUp)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_Z);

      if (KeyHeld(input, SDL_SCANCODE_LSHIFT)) {
        Redo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      } 
      else {
        Undo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
    }

    if (KeyPressed(input, SDL_SCANCODE_RIGHT) ||
        KeyHeldForTime(input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)) {

      ResetKeyHeldTime(input, SDL_SCANCODE_RIGHT);
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] = {1, 0};
    } 
    else if (KeyPressed(input, SDL_SCANCODE_LEFT) ||
               KeyHeldForTime(input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)) {

      ResetKeyHeldTime(input, SDL_SCANCODE_LEFT);
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] = {-1, 0};
    } 
    else if (KeyPressed(input, SDL_SCANCODE_UP) ||
               KeyHeldForTime(input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)) {

      ResetKeyHeldTime(input, SDL_SCANCODE_UP);
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] = {0, -1};
    } 
    else if (KeyPressed(input, SDL_SCANCODE_DOWN) ||
               KeyHeldForTime(input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)) {

      ResetKeyHeldTime(input, SDL_SCANCODE_DOWN);
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] = {0, 1};
    }
    
    bool areEntitiesMoving = false;
    for  (int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++) {
      Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];

      if (HasBehaviour(entity, CAN_MOVE) && IsMoving(entity)) {
        //SDL_Log("Entity is moving! x: %d, xPrev: %d, progress: %f", entity->x,
        //        entity->xPrev, entity->progress01);
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
      if(gameplay->inputBufferReadCount == gameplay->inputBufferWriteCount) {
        return;
      }

      gameplay->commandBuffer->timestamp += 1;

      for (int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++) {
        Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];

        if (HasBehaviour(entity, Behaviour::IS_PUSHING)) {
          RemoveBehaviour(entity, Behaviour::IS_PUSHING);
        }

        if (HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))) {
          if (HasBehaviour(entity, (Behaviour)Behaviour::IS_PETRIFIED)) {
            continue;
          }

          int xDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].x;
          int yDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].y;

          Direction newFacing = DirectionFromXY(xDir, yDir);
          if (newFacing != entity->facing) {
            RotateCommand rotate(entity, entity->facing, newFacing);
            Push(gameplay->commandBuffer, rotate, GetCurrentLevel(gameplay));
          }

          TryMove(entity, GetCurrentLevel(gameplay), gameplay->commandBuffer, xDir, yDir, entity->strength);
        } 
      }

      gameplay->inputBufferReadCount++;
    }
  }

  void Initialize(GameData* gameData, SDL_Window* window,  SDL_Renderer* renderer) {

    DEV::Initialize(window, renderer);
    AssetManagement::LoadAllSprites(gameData->spriteBuffer, renderer);
    gameData->imGuiContext = ImGui::GetCurrentContext();

    SDL_Texture* blackfade = GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);
    
    InitializeGame(&gameData->scenes.gameplay, gameData->arenaLevels);
    ChangeScene(gameData, SCENE_TYPES::GAME);
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
    Gameplay* gameplay = &gameData->scenes.gameplay;
    Titlescreen* titleScreen = &gameData->scenes.titleScreen;
    EditorData* editorData = &gameData->editorData;
    Transition* transition = &gameData->transition;
    
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (KeyPressed(&gameData->input, SDL_SCANCODE_F2)) {
      editorData->editLevel = !editorData->editLevel;
    }

    if (editorData->editLevel) {
      EDITOR::Update(&editorData->editor, &gameData->input, GetCurrentLevel(gameplay), gameplay->commandBuffer);
    }

    if (KeyPressed(&gameData->input, SDL_SCANCODE_5)) {
      ChangeScene(gameData, SCENE_TYPES::TITLESCREEN);
    }
    
    if (transition->state != Transition::Inactive) {
      transition->fadeTimeElapsed += dt;
      
      if (transition->fadeTimeElapsed >= transition->fadeTimeDuration) {
        transition->fadeTimeElapsed = 0;
        
        switch (transition->state) {
          case Transition::Inactive:
            break;
          case Transition::FadeTo:
            transition->state = Transition::FadeFrom;
            break;
          case Transition::FadeFrom:
            transition->state = Transition::Inactive;
            break;
        }
      }
    }

    switch (gameData->sceneCurrent) {
      case SCENE_TYPES::TITLESCREEN: {
        UpdateTitleScreen(titleScreen, dt);
        
        if (AnyKeyPressed(&gameData->input)) {
          if (transition->state == Transition::FadeTo || transition->state == Transition::Inactive) {
            ChangeScene(gameData, SCENE_TYPES::GAME);
          }
        }
      
        break;
      }
      case SCENE_TYPES::MAINMENU: {
        
        break;
      }
      case SCENE_TYPES::GAME: {
        UpdateGame(gameplay, &gameData->input, dt);
        break;
      }
      case SCENE_TYPES::NONE: {
        assert(false);
        break;
      }
    case SCENE_TYPES::CREDITS:
      break;
    }
  }

  void Draw(GameData* gameData, SDL_Renderer* renderer) {
    DEV::PreDraw(gameData->imGuiContext);
    
    SDL_SetRenderDrawColor(renderer, 120, 70, 120, 255);
    SDL_RenderClear(renderer);

    switch (gameData->transition.state) {
      case Transition::Inactive:
        DrawScene(gameData, gameData->sceneCurrent, renderer);
        break;
      case Transition::FadeTo: {
        DrawScene(gameData, gameData->scenePrevious, renderer);
        float alpha = gameData->transition.fadeTimeElapsed / gameData->transition.fadeTimeDuration;
        RenderSpriteWorld(GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer), renderer, &gameData->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }
      case Transition::FadeFrom: {
        DrawScene(gameData, gameData->sceneCurrent, renderer);
        float alpha = 1 - gameData->transition.fadeTimeElapsed / gameData->transition.fadeTimeDuration;
        RenderSpriteWorld(GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer), renderer, &gameData->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }
    }
    
    //RenderLevel(gameData, renderer);
    //RenderEntities(gameData, renderer);

    DEV::Draw(gameData, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
  }
}
