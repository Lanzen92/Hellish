#include <windows.h>
#include <fileapi.h>
#include <timeapi.h>
#include <cstdio>
#include <fstream>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_timer.h"

#include "arena.h"
#include "common.h"
#include "gameState.h"
#include "input.h"

SDL_Window* window;
SDL_Renderer* renderer;

Uint64 NOW;
Uint64 PREV;

//Def functions (singatures) from DLL import.
typedef void (*Function_Initialize) (GameData* data, SDL_Window* window, SDL_Renderer* renderer);
typedef bool (*Function_HandleEvents) (GameData* data, SDL_Event event);
typedef void (*Function_Update) (GameData* data, float dt);
typedef void (*Function_Draw) (GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit) (SDL_Renderer* renderer);

// Function pointers.
constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_QUIT = "OnQuit";

constexpr const char* NAME_OF_DLL = "Hellish_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "Hellish_temp.dll";

struct DLL_INFO {
  HMODULE dll;
  FILETIME timestamp;
  Function_Initialize initialize;
  Function_HandleEvents handleEvents;
  Function_Update update;
  Function_Draw draw;
  Function_OnQuit quit;
};

FILETIME GetTimestamp() {
  WIN32_FIND_DATA data;
  HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
  FILETIME timeOfLastChange = data.ftLastWriteTime;
  FindClose(handle);
  
  return timeOfLastChange;
}



// Load DLL and also load the functions from DLL.
bool LoadDLL(DLL_INFO* info, int depth = 0) {
  printf("Loading DLL...\n");

  if (depth > 20) {
    printf("Failed to write temp DLL...\n");
    return false;
  }

  bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false);

  if (!success) {
    Sleep(50);
    return LoadDLL(info, depth + 1);
  }
  else {
    printf("Dll loaded sucessfully!\n");
  }
  
  info->dll = LoadLibrary(NAME_OF_TEMP_DLL);

  if (info->dll == nullptr) {
    printf("Could not load DLL...\n");
    return false;
  }

  info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);
  info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENT);
  info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);
  info->draw  = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);
  info->quit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_QUIT);

  info->timestamp = GetTimestamp();
  return true;
}
//Handles unload dll
//frees and deletes the temp-dll to allow a "refresh".
void UnloadDLL(DLL_INFO* info) {
  FreeLibrary(info->dll);
  info->dll = nullptr;
  DeleteFile(NAME_OF_TEMP_DLL);
}

//Check if the timestamp of the dll has changed.
//If it has, unload and load the new.
void DLL_CheckStatus(DLL_INFO* dll) {
  FILETIME timestamp = GetTimestamp();

  bool isTimeChanged = CompareFileTime(&dll->timestamp, &timestamp) != 0;

  if (isTimeChanged) {
    UnloadDLL(dll);
    LoadDLL(dll);
  }
}

//Allocate memory
//Gets void* to the free block
//Which has been allocated to the program.
void* AllocateGameMemory() {
  void* blob = malloc(GAME_MEMORY_ALLOWANCE);

  if(blob == nullptr) {
    printf("Fatal error: Could not allocate memory...\n");
    return nullptr;
  }

  printf("Memory allocated\n");
  return blob;
}

//Prepare the SDL components
//Create window and renderer.

//Change resolution:
// break out w / h to variables
// make them "configurable"
void SDL_Setup() {
  SDL_Init(SDL_INIT_EVENTS);
  window = SDL_CreateWindow("TheUltimateGame", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, NULL);
}

//Get delta
void CalculateDeltaTime(float& dt, float scaler) {
  NOW = SDL_GetTicksNS();
  dt = NOW - PREV;
  dt = SDL_NS_TO_SECONDS(dt);
  dt *= scaler;
  PREV = NOW;
}

 // Calculate how long CPU can sleep before next frame is scheduled.
 // Using the FPS-var in common.h
void CalculateRemainingFrameTime_MS(double* milliSeconds) {
  Uint64 frameEndTimeNS = SDL_GetTicksNS();
  double frameTimeSpentNS = frameEndTimeNS - PREV;
  double frameTimeSpentMS = frameTimeSpentNS / 1e6;
  *milliSeconds = FRAME_TIME_MS - frameTimeSpentMS;
}

//Writing gamestate to bin file.
void StoreGameState (Memory::Arena* arena) {
  std::ofstream file("temp_state.bin", std::ios::binary);
  file.write(reinterpret_cast<const char*>(arena->base), arena->size);
}

//Reading gamestate from bin file. 
void RetrieveGameState(Memory::Arena* arena) {
  std::ifstream file("temp_state.bin", std::ios::binary);
  file.read(reinterpret_cast<char*>(arena->base), arena->size);
}

int main() {

  printf("Initializing... \n");
  
  //Prepare the memory for usage.
  void* gameMemory = AllocateGameMemory();

  if (gameMemory == nullptr) {
    return 1;
  }

  // ----- MemoryAllocation  ------

  printf("Allocating memory..  \n");

  //Set up CPU / RAM memory
  Memory::Arena* arenaMain = new Memory::Arena();
  Memory::Initialize(arenaMain, gameMemory, GAME_MEMORY_ALLOWANCE);
  GameData* gameData = ALLOC_ARRAY(arenaMain, GameData, 1)
  gameData->arenaMain = arenaMain;

  int SPRITE_COUNT = 256;
  size_t IMAGE_ARENA_SIZE = MEGABYTES(2);
  gameData->arenaImages = Memory::CreateSubArena(arenaMain, IMAGE_ARENA_SIZE);
  gameData->spriteBuffer = ALLOC_ARRAY(gameData->arenaImages, Sprite, SPRITE_COUNT)
  gameData->tilesetBuffer = ALLOC_ARRAY(gameData->arenaImages, Tileset,(int)TILESETS::COUNT)

  gameData->editorData.fpsBufferCount = 500;
  gameData->editorData.fpsBuffer = ALLOC_ARRAY(gameData->arenaMain, float, gameData->editorData.fpsBufferCount);

  gameData->arenaScratch = Memory::CreateSubArena(arenaMain, KILOBYTES(256));

  size_t INPUT_ARENA_SIZE = 0;
  INPUT_ARENA_SIZE += sizeof(bool) * SDL_SCANCODE_COUNT * 2;
  INPUT_ARENA_SIZE += sizeof(float) * SDL_SCANCODE_COUNT;
  INPUT_ARENA_SIZE += 128;
  gameData->arenaInputs = Memory::CreateSubArena(arenaMain, INPUT_ARENA_SIZE);

  gameData->input.keysCurrent = ALLOC_ARRAY(gameData->arenaInputs, bool, SDL_SCANCODE_COUNT);
  gameData->input.keysPrevious = ALLOC_ARRAY(gameData->arenaInputs, bool, SDL_SCANCODE_COUNT)
  gameData->input.keysHeldTime = ALLOC_ARRAY(gameData->arenaInputs, float, SDL_SCANCODE_COUNT)
  gameData->input.mouseHeldTime = ALLOC_ARRAY(gameData->arenaInputs, float, (int)MouseButtons::COUNT)

  gameData->arenaLevels = Memory::CreateSubArena(arenaMain, MEGABYTES(3));
  gameData->arenaEntities = Memory::CreateSubArena(gameData->arenaLevels, MEGABYTES(1));
  gameData->arenaCommands = Memory::CreateSubArena(gameData->arenaLevels, MEGABYTES(1));

  Gameplay* gameplay = &gameData->scenes.gameplay;
  gameplay->inputBufferCapacity = 50;
  gameplay->inputBuffer = ALLOC_ARRAY(gameData->arenaLevels, Position, gameplay->inputBufferCapacity)

  gameplay->levelCount = 256;
  gameplay->levels = ALLOC_ARRAY(gameData->arenaLevels, LevelData, gameplay->levelCount)
  gameplay->commandBuffer = ALLOC(gameData->arenaCommands, CommandBuffer)
  gameplay->commandBuffer->capacity = 2000;
  gameplay->commandBuffer->allCommands = ALLOC_ARRAY(gameData->arenaCommands, AnyCommand, gameplay->commandBuffer->capacity)

  printf("Allocation done \n");
  // ----- MemoryAllocation end ------

  MMRESULT result = timeBeginPeriod(1);
  if (result == TIMERR_NOCANDO) {
    printf("Could not increase timer resolution\n");
    Sleep(2000);
    return 3;
  }
 
  DLL_INFO dll;
  bool dllSuccessfullyLoaded = LoadDLL(&dll);

  if (!dllSuccessfullyLoaded) {
    return 2;
  }

  SDL_Setup();
  dll.initialize(gameData, window, renderer);
  
  gameData->running = true;
  float dt;
  float dtScaler = 1;
  gameData->dt = &dt;
  gameData->dtScaler = &dtScaler;
  

  printf("Initialization done - Jumping to gameloop \n");

  while (gameData->running) {
    //Each frame, check if dll has changed. 
    DLL_CheckStatus(&dll);

    Reset(gameData->arenaScratch);

    //Get Delta
    CalculateDeltaTime(dt, dtScaler);
    
    SDL_Event event;

    //Listen to events, and close if bool changes to false.
    while(SDL_PollEvent(&event)) {
      gameData->running = dll.handleEvents(gameData, event);

      if (!gameData->running) {
        break;
      }

      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_F9) {
          printf("Store state\n");
          StoreGameState(arenaMain);
        }
        if (event.key.key == SDLK_F10) {
          printf("Retrieve state\n");
          RetrieveGameState(arenaMain);
        }
      }
    }

    //Mouse input
    gameData->input.keysCurrent = SDL_GetKeyboardState(nullptr);
    float* deltaX = &gameData->input.mouseXDelta;
    float* deltaY = &gameData->input.mouseYDelta;
    *deltaX = gameData->input.mouseX;
    *deltaY = gameData->input.mouseY;
    
    gameData->input.mouseCurrent = SDL_GetMouseState(&gameData->input.mouseX, &gameData->input.mouseY);
    *deltaX = gameData->input.mouseX - *deltaX;
    *deltaY = gameData->input.mouseY - *deltaY;
    
    float dx = *deltaX;
    float dy = *deltaY;
    gameData->input.mouseMagnitude = std::sqrt(dx * dx + dy * dy);
    
    dll.update(gameData, dt);
    
    UpdateKeys(&gameData->input, dt);
    UpdateMouse(&gameData->input, dt);

    dll.draw(gameData, renderer);

    //Goal is to keep it as close as possible of the preset FPS. (common.h)
    double timeToSleepMS;
    CalculateRemainingFrameTime_MS(&timeToSleepMS);
    if (timeToSleepMS > 0) {
      //If greater than 1, subrtact 1. SDL may not sleep for exactly the amount
      // of MS desired.
      if (timeToSleepMS > 1) {
        SDL_Delay(timeToSleepMS - 1);
      }
      
      //Loop the last bit to be able to time the frame neatly.
      while (timeToSleepMS > 0) {
        CalculateRemainingFrameTime_MS(&timeToSleepMS);
      }
    }
    // else {
    //   printf("Missed frame.. \n");
    // }
  }
  
  dll.quit(renderer);
  SDL_Quit();
  return 0;
}
