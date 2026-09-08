#include <cstring>

#include "SDL3/SDL_scancode.h"

#include "input.h"

bool KeyPressed(const Input* input, SDL_Scancode key) {
  if (input->keysPrevious == nullptr){
    return input->keysCurrent[key];
  }
  
  return input->keysCurrent[key] && !input->keysPrevious[key];
}

bool KeyHeld(const Input* input, SDL_Scancode key) {
  if (input->keysPrevious == nullptr){
    return false;    
  }
  
  return input->keysCurrent[key] && input->keysPrevious[key];
}

bool KeyReleased(const Input* input, SDL_Scancode key) {
  if (input->keysPrevious == nullptr){
    return false;
  }
  
  return !input->keysCurrent[key] && input->keysPrevious[key];
}

bool KeyHeldForTime(const Input* input, SDL_Scancode key, float minLength) {
  return input->keysHeldTime[key] >= minLength;
}

void UpdateKeys(Input* input, float dt) {
  for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
    if (input->keysCurrent[i]) {
      input->keysHeldTime[i] += dt;
    }
    else {
      input->keysHeldTime[i] = 0;
    }
  }
  memcpy((void*)input->keysPrevious, input->keysCurrent, SDL_SCANCODE_COUNT * sizeof(bool));
}

void ResetKeyHeldTime(Input *input, SDL_Scancode key) {
  input->keysHeldTime[key] = 0;
}

void ResetAll(Input* input) {
  memset((void*)input->keysCurrent, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keysPrevious, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keysHeldTime, 0, sizeof(float) * SDL_SCANCODE_COUNT);
}
