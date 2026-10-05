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
#include "mainmenu.h"
#include "rendering.h"
#include "spriteLibrary.h"

extern "C" {

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
    //ENTITY_ID StepIntoTileId = (ENTITY_ID)GetCellID(levelData, testX, testY);

    if (stepIntoEntity == nullptr) {
      if (IsWalkable(testX, testY, levelData)) {
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

  void StartLevel(Gameplay* gameplay, Arena* arenaCommands, Arena* arenaEntities) {
    ResetCommandBuffer(gameplay->commandBuffer);
    Reset(arenaCommands);
    CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], arenaEntities);
    gameplay->activePlayerIndex = 0;
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
      gameData->scenes.titleScreen.displayTimer = 0.0f;
      break;
    }
    case SCENE_TYPES::MAINMENU: {
      SDL_Log("Changed to MainMenu");
      gameData->transition.fadeTimeDuration = 1;
      break;
    }
    case SCENE_TYPES::GAME: {
      SDL_Log("Changed to Game");
      gameData->transition.fadeTimeDuration = 0.5f;
      Gameplay* gameplay = &gameData->scenes.gameplay;
      assert(gameplay->initialized);
      
      //Reset to level 1, and set gameWon to false.
      if (gameData->scenes.gameplay.gameWon == true) {
        gameData->scenes.gameplay.currentLevelIndex = 0;
        gameData->scenes.gameplay.gameWon = false;
      }
      
      StartLevel(gameplay, gameData->arenaCommands, gameData->arenaEntities);
      break;
    }
    case SCENE_TYPES::CREDITS: {
      SDL_Log("Changed to Credits");
      gameData->transition.fadeTimeDuration = 1;
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
        RenderBackground(background, renderer);
        break;
      }
      case SCENE_TYPES::MAINMENU: {
        DrawMenu(&gameData->scenes.mainMenu, renderer, gameData->spriteBuffer, &gameData->input);
        RenderText(&gameData->font, "Main Menu", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 300, Alignment::Centered, Type::Header);
        break;
      }
      case SCENE_TYPES::GAME: {
        RenderLevel(gameData, renderer);
        RenderEntities(gameData, renderer);
        break;
      }
      case SCENE_TYPES::CREDITS: {
        Sprite* background = GetSprite(SPRITE_ID::TitleScreenBackground, gameData->spriteBuffer);
        RenderBackground(background, renderer);
        
        RenderText(&gameData->font, "Credits", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 460, Alignment::Centered, Type::Header);
        
        RenderText(&gameData->font, "Programing", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 400, Alignment::Centered);
        RenderText(&gameData->font, "Robin Lanz", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 360, Alignment::Centered);
        
        RenderText(&gameData->font, "Art", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 300, Alignment::Centered);
        RenderText(&gameData->font, "Max Friberg", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 - 260, Alignment::Centered);
        
        RenderText(&gameData->font, "Idea and original code by", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 + 200, Alignment::Centered);
        RenderText(&gameData->font, "Max Friberg", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 + 240, Alignment::Centered);
        
        RenderText(&gameData->font, "Press any key to go to Main Menu", renderer, &gameData->camera, SCREEN_WIDTH / 2.0,SCREEN_HEIGHT / 2.0 + 300, Alignment::Centered);
        break;
        
      }
      case SCENE_TYPES::NONE:
        assert(false);
        break;
      }
  }

  void InitializeGame(Gameplay* gameplay, Arena* arenaLevels, Tileset* tilesetBuffer) {
    assert(gameplay->initialized == false);
        
    gameplay->currentLevelIndex = 0;
    gameplay->loadedLevels = 0;
    gameplay->gameWon = false;
    //CreateLevel(arenaLevels, &gameplay->levels[0], &tilesetBuffer[(int)TILESETS::DUNGEON], "assets/maps/testing_goal.tmj");

    CreateLevel(arenaLevels, &gameplay->levels[0], &tilesetBuffer[(int)TILESETS::DUNGEON], "assets/maps/level_01.tmj");
    CreateLevel(arenaLevels, &gameplay->levels[1], &tilesetBuffer[(int)TILESETS::DUNGEON], "assets/maps/level_02.tmj");
    gameplay->loadedLevels = 2;
    
    gameplay->initialized = true;
  }

  void Initialize(GameData* gameData, SDL_Window* window,  SDL_Renderer* renderer) {
    *gameData->ticksTotal = 0;
    DEV::Initialize(window, renderer);
    AssetManagement::LoadAllSprites(gameData->spriteBuffer, renderer);
    gameData->imGuiContext = ImGui::GetCurrentContext();
    
    AssetManagement::LoadFont(renderer, "assets/fonts/ByteBounce.ttf", &gameData->font, 48);
    
    InitializeAudioSystem(&gameData->audioSystem, gameData->arenaMain);
    AssetManagement::LoadAllSFX(&gameData->audioSystem);
    
    AssetManagement::LoadAllTilesets(gameData->tilesetBuffer, gameData->arenaImages);

    SDL_Texture* blackfade = GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);
    
    InitializeGame(&gameData->scenes.gameplay, gameData->arenaLevels, gameData->tilesetBuffer);
    InitializeMenu(&gameData->scenes.mainMenu, gameData->spriteBuffer, &gameData->font, gameData->arenaMain);
    
    PlaySong(SONG_ID::THEME);
    
    ChangeScene(gameData, SCENE_TYPES::TITLESCREEN);
  }

  void UpdateGame(Gameplay* gameplay, Input* input, Arena* arenaScratch, Arena* arenaCommands, Arena* arenaEntities, const float dt) {
    float undoSpeedUp = std::lerp(1.0, 0.15, (gameplay->commandBuffer->head - gameplay->commandBuffer->index) * (1.0/30.0));
    undoSpeedUp = std::max<double>(undoSpeedUp, 0.15);
    
    if (KeyPressed(input, SDL_SCANCODE_R)) {
      StartLevel(gameplay, arenaCommands, arenaEntities);
      return;
    }
    
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
    
    bool areEntitiesActing = false;
    LevelData* levelData = GetCurrentLevel(gameplay);
    Entity* entityBuffer = levelData->entityBuffer;
    
    for (int i = 0; i < levelData->entityCount; i++){
      if(IsActing(&entityBuffer[i])){
        areEntitiesActing = true;
        break;
      }
    }
    
    for (int i = 0; i < levelData->goalCount; i++) {
      Entity* entity = GetEntity(levelData, levelData->goals[i].x, levelData->goals[i].y);
      if(entity != nullptr && !IsActing(entity)){
        levelData->goals[i].blinkTimer += dt;
      }
      else{
        levelData->goals[i].blinkTimer = 0;
      }
    }
    
    if(levelData->goalCount > 0) {
      int goals_reached = 0;
      for (int i = 0; i < levelData->goalCount; i++) {
        Goal goal = levelData->goals[i];
        Entity* entity = GetEntity(levelData, goal.x, goal.y);
        if(entity == nullptr){
          break;
        }
        else if(HasBehaviour(entity, Behaviour::IS_PLAYER)){
          goals_reached++;
        }
      }
      
      if (goals_reached == levelData->goalCount) {
        if (gameplay->currentLevelIndex == gameplay->loadedLevels - 1) {
          gameplay->gameWon = true; 
          return; 
        }      
        
        gameplay->currentLevelIndex++;
        StartLevel(gameplay, arenaCommands, arenaEntities);
        return;
      }
    }
    
    for (int i = 0; i < levelData->entityCount; i++){
      Entity* entity = &entityBuffer[i];
      
      if(!entity->active) continue;
      
      switch(entity->action){
      case Actions::NONE:
        continue;
      case Actions::MOVING:
        entity->progress01 += MOVE_SPEED * dt;
        break;
      case Actions::ROTATING:
        entity->progress01 += 8 * dt;
        break;
      }
    }
    
    for  (int i = 0; i < levelData->entityCount; i++) {
      Entity* entity = &entityBuffer[i];
      
      if (entity->progress01 >= 1) {
        entity->xPrev = entity->x;
        entity->yPrev = entity->y;
        entity->facingPrevious = entity->facingCurrent;
        entity->action = Actions::NONE;
        entity->progress01 = 0;
        
        if(HasBehaviour(entity, Behaviour::IS_PUSHING)){
          RemoveBehaviour(entity, Behaviour::IS_PUSHING);
        }
      }
    }

    int playerCount = 0;
    for (int i = 0; i < levelData->entityCount; i++) {
      if(entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&levelData->entityBuffer[i], (Behaviour)(IS_PLAYER))){
        playerCount++;
      }
    }

    int index = 0;
    gameplay->activePlayerBuffer = ALLOC_ARRAY(arenaScratch, Entity*, playerCount)
    for (int i = 0; i < levelData->entityCount; i++) {
      if(entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&entityBuffer[i], (Behaviour)(IS_PLAYER))){
        gameplay->activePlayerBuffer[index++] = &entityBuffer[i];
      }
    }
    
    if(areEntitiesActing == false && KeyPressed(input, SDL_SCANCODE_X) && playerCount > 0){
      SwapActiveEntityCommand swap(&gameplay->activePlayerIndex, playerCount);
      Push(gameplay->commandBuffer, swap, GetCurrentLevel(gameplay));
      gameplay->commandBuffer->timestamp += 1;
    }
    
    if (areEntitiesActing) 
      return;
      
    if(gameplay->inputBufferReadCount == gameplay->inputBufferWriteCount)
      return;
    
    Entity* entity = GetActiveEntity(gameplay);
    
    if(!HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE)))
      return;
    
    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED))
      return;
    
    int xDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].x;
    int yDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].y;

    Direction newFacing = DirectionFromXY(xDir, yDir);
    if (newFacing != entity->facingCurrent) {
      RotateCommand rotate(entity, entity->facingCurrent, newFacing);
      Push(gameplay->commandBuffer, rotate, GetCurrentLevel(gameplay));
    }

    if(!IsActing(entity)){
      bool moved = TryMove(entity, levelData, gameplay->commandBuffer, xDir, yDir, entity->strength);
      
      if (moved) {
        PlaySFX(SFX_ID::JUMP);
      }
      
      gameplay->commandBuffer->timestamp += 1;
      gameplay->inputBufferReadCount++;
    }
  }

  void Update(GameData* gameData, float dt) {
    *gameData->ticksTotal += 1;
    
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
        
        // Change scene after 5 sec, or if any key is pressed.
        titleScreen->displayTimer += dt;
        if (AnyKeyPressed(&gameData->input) || (titleScreen->displayTimer >= 5.0f)) {
          if (transition->state == Transition::FadeTo || transition->state == Transition::Inactive) {
            ChangeScene(gameData, SCENE_TYPES::MAINMENU);
          }
        }
      
        break;
      }
      case SCENE_TYPES::MAINMENU: {
        UpdateMenu(gameData);
        break;
      }
    case SCENE_TYPES::GAME: {
      if (transition->state == Transition::FadeFrom || transition->state == Transition::Inactive) {
        UpdateGame(gameplay, &gameData->input, gameData->arenaScratch, gameData->arenaCommands, gameData->arenaEntities, dt);
      }
      
      if (gameplay->gameWon) {
        if (transition->state == Transition::Inactive) {
          ChangeScene(gameData, SCENE_TYPES::CREDITS);
        }
      }
      break;
    }
      case SCENE_TYPES::CREDITS:
        if (AnyKeyPressed(&gameData->input)) {
          if (transition->state == Transition::FadeTo || transition->state == Transition::Inactive) {
            ChangeScene(gameData, SCENE_TYPES::MAINMENU);
          }
        }
        break;
      
      case SCENE_TYPES::NONE: {
        assert(false);
        break;
      }
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
        RenderSpriteWorld(GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer), renderer, &gameData->camera, 0, 0, SCREEN_WIDTH, alpha);        break;
      }
      case Transition::FadeFrom: {
        DrawScene(gameData, gameData->sceneCurrent, renderer);
        float alpha = 1 - gameData->transition.fadeTimeElapsed / gameData->transition.fadeTimeDuration;
        RenderSpriteWorld(GetSprite(SPRITE_ID::Black1x1, gameData->spriteBuffer), renderer, &gameData->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }
    }
    
    DEV::Draw(gameData, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
  }
}

