
#include "levelEditor.h"
#include "camera.h"
#include "imgui/imgui.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"

namespace EDITOR {
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer) {
    ImGui::Begin("Placeable objects");
    ImVec2 size = {32, 32};

    if (ImGui::ImageButton("Ground", (ImTextureID)GetSpriteFromID(ID::GROUND, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ID::GROUND;
    }

    ImGui::SameLine();
    if (ImGui::ImageButton("Wall", (ImTextureID)GetSpriteFromID(ID::WALL, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ID::WALL;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Rock", (ImTextureID)GetSpriteFromID(ID::ROCK, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ID::ROCK;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Demon", (ImTextureID)GetSpriteFromID(ID::DEMON, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ID::DEMON;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Medusa", (ImTextureID)GetSpriteFromID(ID::MEDUSA, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ID::MEDUSA;
    }

    ImGui::End();
  }

  void PlaceObject(const int x, const int y, Editor* editor, LevelData* levelData) {
    if (editor->objectToPlaceId == ID::GROUND || editor->objectToPlaceId == ID::WALL) {
      levelData->cells[y * levelData->w + x] = (int)editor->objectToPlaceId;
    }
    else {
      AddEntity(editor->objectToPlaceId, x, y, levelData);
    }
  }
  
  void Update(Editor* editor, Input* input, LevelData* levelData) {
    if (MousePressed(input, MouseButtons::LEFT)) {
      if (camera::GetIsPointInsideGrid(input->mouseX, input->mouseY, levelData)) {
        int x;
        int y;

        camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
        PlaceObject(x, y, editor, levelData);
        
      }
      else if (MousePressed(input, MouseButtons::RIGHT)) {
        if (camera::GetIsPointInsideGrid(input->mouseX, input->mouseY, levelData)) {
          int x;
          int y;

          camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
          RemoveEntity(x, y, levelData);
        }
      }
    }
  }
  
  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* levelData, Camera* camera, Sprite* spriteBuffer) {
    int x;
    int y;

    camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
    Sprite* preview = GetSpriteFromID(editor->objectToPlaceId, spriteBuffer);

    if (preview != nullptr) {
      RenderSpriteGrid(preview, levelData, renderer, camera, x, y, 1, 128.0f); 
    }
    else
    {
      SDL_Log("DrawPreview / No preview sprite found for ID: %d",
              (int)editor->objectToPlaceId);
    }
  }
}
