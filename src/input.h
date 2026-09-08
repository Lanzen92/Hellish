#pragma once

#include "SDL3/SDL_scancode.h"

struct Input {
  const bool* keysCurrent;
  const bool* keysPrevious;
  float* keysHeldTime;
};


bool KeyPressed(const Input* input, SDL_Scancode key);
bool KeyHeld(const Input* input, SDL_Scancode key);
bool KeyReleased(const Input* input, SDL_Scancode key);
bool KeyHeldForTime(const Input* input, SDL_Scancode key, float minLength);
void UpdateKeys(Input* input, float dt);
void ResetKeyHeldTime(Input* input, SDL_Scancode key);
void ResetAll(Input*);
