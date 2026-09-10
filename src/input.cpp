#include <cstring>

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
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

SDL_MouseButtonFlags ButtonToFlag(MouseButtons button) {
  switch (button) {
    case MouseButtons::LEFT:
      return SDL_BUTTON_LMASK;
    case MouseButtons::MIDDLE:
      return SDL_BUTTON_MMASK;
    case MouseButtons::RIGHT:
      return SDL_BUTTON_RMASK;
  }
}

bool MousePressed(const Input* input, MouseButtons button) {
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouseCurrent & flag) != 0 && (input->mousePrevious & flag) == 0;
}

bool MouseReleased(const Input* input, MouseButtons button) {
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouseCurrent & flag) == 0 && (input->mousePrevious & flag) != 0;
}

bool MouseHeld(const Input* input, MouseButtons button) {
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouseCurrent & flag) != 0 && (input->mousePrevious & flag) != 0;
}

bool MouseHeldForTime(const Input* input, MouseButtons button, float minLength) {
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouseHeldTime[flag] >= minLength);
}

void UpdateMouse(Input* input, float dt) {
  if (MouseHeld(input, MouseButtons::LEFT)) {
    input->mouseHeldTime[(int)MouseButtons::LEFT] += dt;
  }
  else {
    input->mouseHeldTime[(int)MouseButtons::LEFT] = 0;
  }
  
  if (MouseHeld(input, MouseButtons::MIDDLE)) {
    input->mouseHeldTime[(int)MouseButtons::MIDDLE] += dt;
  }
  else {
    input->mouseHeldTime[(int)MouseButtons::MIDDLE] = 0;
  }
  
  if (MouseHeld(input, MouseButtons::RIGHT)) {
    input->mouseHeldTime[(int)MouseButtons::RIGHT] += dt;
  }
  else {
    input->mouseHeldTime[(int)MouseButtons::RIGHT] = 0;
  }

  input->mousePrevious = input->mouseCurrent;
}
