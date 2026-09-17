#pragma once

#include "camera.h"
#include "entity.h"
#include "input.h"
#include "level.h"
#include "spriteLibrary.h"

struct Editor {
  ID objectToPlaceId;
};

namespace EDITOR {
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer);
  void PlaceObject(const int x, const int y, Editor* editor, LevelData* levelData);
  void Update(Editor* editor, Input* input, LevelData* levelData);
  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* levelData, Camera* camera, Sprite* spriteBuffer);
}
