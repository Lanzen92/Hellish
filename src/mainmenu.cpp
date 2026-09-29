#include "mainmenu.h"

#include "button.h"
#include "arena.h"
#include "common.h"
#include "gameState.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"

#include <cassert>

void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, Memory::Arena* arenaMain) {
  assert(mainmenu->initialized == false);
  
  mainmenu->buttonCount = 2;
  mainmenu->buttons = ALLOC_ARRAY(arenaMain, Button, mainmenu->buttonCount)
  
  SetupButton(&mainmenu->buttons[0], ButtonType::START_GAME, spriteBuffer, {SCREEN_WIDTH / 2.0,  SCREEN_HEIGHT / 2.0, 200, 80}, ButtonMode::Centered);
  SetupButton(&mainmenu->buttons[1], ButtonType::QUIT, spriteBuffer, {SCREEN_WIDTH / 2.0,  SCREEN_HEIGHT / 2.0 + 100, 200, 80}, ButtonMode::Centered);

  mainmenu->initialized = true;
}

void DrawMenu(MainMenu* mainmenu, SDL_Renderer* renderer, Sprite* spriteBuffer) {
  Sprite* background = GetSprite(SPRITE_ID::MainMenuBackground, spriteBuffer);
  
  float scale = (SCREEN_HEIGHT / ((float)background->height * UPSCALE_FACTOR));
  RenderSpriteWorld(GetSprite(SPRITE_ID::MainMenuBackground, spriteBuffer), renderer, NULL, 0.0f, 0.0f, scale);
  
  for (int i = 0; i < mainmenu->buttonCount; i++) {
    Button* button = mainmenu->activeButtons[i];
    RenderButton(button, i == mainmenu->activeButtonIndex, renderer);
  }
}

void UpdateMenu(GameData* gameData) {
  MainMenu* mainmenu = &gameData->scenes.mainMenu;
  Input* input = &gameData->input;
  mainmenu->activeButtonsCount = GetActiveButtonCount(mainmenu->buttons, mainmenu->buttonCount);
  
  if(mainmenu->activeButtonsCount == 0){
    return;
  }
  
  mainmenu->activeButtons = ALLOC_ARRAY(gameData->arenaScratch, Button*, mainmenu->activeButtonsCount)
  int index = 0;
  for (int i = 0; i < mainmenu->buttonCount; i++) {
    Button* button = &mainmenu->buttons[i];
    if(button->isActive){
      mainmenu->activeButtons[index] = button;
      index += 1;
    }
  }
  int* buttonIndex = &mainmenu->activeButtonIndex;
  bool anyHoveredOver = false;
  bool mouseMoving = input->mouseMagnitude > 0.1;
  
  if(mouseMoving) {
    for (int i = 0; i < mainmenu->activeButtonsCount; i++) {
      Button* button = mainmenu->activeButtons[i];
      if(IsHoveredOver(button, input->mouseX, input->mouseY)){
        anyHoveredOver = true;
        *buttonIndex = i;
        break;
      }
    }
  }
  
  bool up = KeyPressed(input, SDL_SCANCODE_UP);
  bool down = KeyPressed(input, SDL_SCANCODE_DOWN);
  
  if (up || down) {
    int direction = up ? 1 : -1;
    *buttonIndex += direction + mainmenu->activeButtonsCount;
    *buttonIndex = *buttonIndex % mainmenu->activeButtonsCount;
  }
  
  Button* selected = mainmenu->activeButtons[*buttonIndex];
  
  if (selected != nullptr) {
    if (KeyPressed(input, SDL_SCANCODE_RETURN)) {
      PressButton(mainmenu->activeButtons[*buttonIndex], gameData);
      return;
    }
  }
  
  if (IsHoveredOver(selected, input->mouseX, input->mouseY)) {
    if (MousePressed(input, MouseButtons::LEFT)) {
      PressButton(selected, gameData);
      return;
    }
  }
}