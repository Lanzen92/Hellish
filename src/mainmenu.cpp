#include "mainmenu.h"

#include "button.h"
#include "arena.h"
#include "common.h"
#include "gameState.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"

#include <cassert>

void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arenaMain) {
  assert(mainmenu->initialized == false);
  
  mainmenu->buttonCount = 2;
  mainmenu->buttons = ALLOC_ARRAY(arenaMain, Button, mainmenu->buttonCount)
  
  SetupButton(&mainmenu->buttons[0], ButtonType::START_GAME, spriteBuffer, {SCREEN_WIDTH / 2.0,  SCREEN_HEIGHT / 2.0, 300, 110}, Alignment::Centered, font, "Start Game");
  mainmenu->buttons[0].isActive = true;
  SetupButton(&mainmenu->buttons[1], ButtonType::QUIT, spriteBuffer, {SCREEN_WIDTH / 2.0,  SCREEN_HEIGHT / 2.0 + 120, 240, 110}, Alignment::Centered, font, "Quit");
  
  mainmenu->backgroundHorizon = GetSprite(SPRITE_ID::MenuHorizon, spriteBuffer);
  mainmenu->backgroundCloudBack = GetSprite(SPRITE_ID::MenuCloudBack, spriteBuffer);
  mainmenu->backgroundCloudFront = GetSprite(SPRITE_ID::MenuCloudFront,spriteBuffer);
  mainmenu->backgroundMiddle = GetSprite(SPRITE_ID::MenuMiddle, spriteBuffer);
  mainmenu->backgroundFront = GetSprite(SPRITE_ID::MenuFront, spriteBuffer);
  
  mainmenu->initialized = true;
}

void DrawMenu(MainMenu* mainmenu, SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input) {

  float scale = (SCREEN_HEIGHT / ((float)mainmenu->backgroundHorizon->height * UPSCALE_FACTOR));
  scale *= 1.2;
  float mouseX = input->mouseX;
  float mouseY = input->mouseY;
  float centerX = SCREEN_WIDTH / 2.0;
  float centerY = SCREEN_HEIGHT / 2.0;
  float offsetX = centerX - mouseX;
  float offsetY = centerY - mouseY;
  
  RenderSpriteWorld(GetSprite(SPRITE_ID::MenuHorizon, spriteBuffer), renderer, NULL, centerX, centerY, scale);
  RenderSpriteWorld(GetSprite(SPRITE_ID::MenuCloudBack, spriteBuffer), renderer, NULL, centerX + (offsetX / 11), centerY + (offsetY / 11), scale);
  RenderSpriteWorld(GetSprite(SPRITE_ID::MenuCloudFront, spriteBuffer), renderer,NULL, centerX + (offsetX / 9), centerY + (offsetY / 9), scale);
  RenderSpriteWorld(GetSprite(SPRITE_ID::MenuMiddle, spriteBuffer), renderer,NULL, centerX + (offsetX / 7), centerY + (offsetY / 7), scale);
  RenderSpriteWorld(GetSprite(SPRITE_ID::MenuFront, spriteBuffer), renderer,NULL, centerX + (offsetX / 5), centerY + (offsetY / 5), scale);
  
  for (int i = 0; i < mainmenu->buttonCount; i++) {
    Button* button = mainmenu->activeButtons[i];
    if (button->isDynamic) 
      RenderButtonDynamic(button, i == mainmenu->activeButtonIndex, renderer);
    else 
      RenderButton(button, i == mainmenu->activeButtonIndex, renderer);
  }
  mainmenu->activeButtonsCount = 0;
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