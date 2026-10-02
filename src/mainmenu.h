#pragma once

#include "input.h"

struct GameData;
struct SDL_Renderer;
struct Button;
struct Sprite;
struct FontAtlas;

namespace Memory {
  struct Arena;
}

struct MainMenu {
  Button* buttons;
  int buttonCount;
  int activeButtonIndex;
  Button** activeButtons;
  int activeButtonsCount;
  bool initialized;
  
  Sprite* backgroundHorizon; 
  Sprite* backgroundCloudBack; 
  Sprite* backgroundCloudFront; 
  Sprite* backgroundMiddle;
  Sprite* backgroundFront; 
};

void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arenaMain);
void UpdateMenu(GameData* gameData);
void DrawMenu(MainMenu* mainmenu, SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input);