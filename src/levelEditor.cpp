#include "SDL3_image/SDL_image.h"
#include "imgui/imgui.h"

#include "levelEditor.h"
#include "camera.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include "command.h"

namespace EDITOR {
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer) {
    ImGui::Begin("Placeable objects");
    ImVec2 size = {32, 32};
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Rock", (ImTextureID)GetSpriteFromID(ENTITY_ID::ROCK, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::ROCK;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Demon", (ImTextureID)GetSpriteFromID(ENTITY_ID::DEMON, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::DEMON;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Golem", (ImTextureID)GetSpriteFromID(ENTITY_ID::GOLEM, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::GOLEM;
    }

    ImGui::SameLine();
    if (ImGui::ImageButton("Medusa", (ImTextureID)GetSpriteFromID(ENTITY_ID::MEDUSA, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::MEDUSA;
    }

    ImGui::SameLine();
    if (ImGui::ImageButton("Siren",(ImTextureID)GetSpriteFromID(ENTITY_ID::SIREN, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::SIREN;
    }

    ImGui::End();
  }

  void PlaceObject(const int x, const int y, Editor* editor, LevelData* levelData, CommandBuffer* commandBuffer) {
    AddCommand ac(editor->objectToPlaceId, x, y);
    Push(commandBuffer, ac, levelData);
  }
  
  void Update(Editor* editor, Input* input, LevelData* levelData, CommandBuffer* commandBuffer) {
    if (MousePressed(input, MouseButtons::LEFT)) {
      if (CAMERA::GetIsPointInsideGrid(input->mouseX, input->mouseY, levelData)) {
        int x;
        int y;

        CAMERA::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
        PlaceObject(x, y, editor, levelData, commandBuffer);
      }
    }
    else if (MousePressed(input, MouseButtons::RIGHT)) {
      if (CAMERA::GetIsPointInsideGrid(input->mouseX, input->mouseY, levelData)) {
        int x;
        int y;

        CAMERA::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
        Entity* entity = GetEntity(levelData, x, y);
        if (entity == nullptr) {
          return;
        }

        RemoveCommand rc(entity);
        Push(commandBuffer, rc, levelData);
        //RemoveEntity(x, y, levelData);
      }
    }
  }
  
  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* levelData, Camera* camera, Sprite* spriteBuffer) {
    int x;
    int y;

    CAMERA::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
    Sprite* preview = GetSpriteFromID(editor->objectToPlaceId, spriteBuffer);

    if (preview != nullptr) {
      RenderEntityOnTile(preview, levelData, renderer, camera, x, y, 1, 0.5); 
    }
  }
}
