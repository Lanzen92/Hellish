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
    ImVec2 size = {64, 64};
    
    ImVec2 uv0 = ImVec2(0.0f, 0.0f);
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Rock", (ImTextureID)GetSprite(SPRITE_ID::Rock, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::ROCK;
    }
    
    ImGui::SameLine();
    if (ImGui::ImageButton("Demon", (ImTextureID)GetSprite(SPRITE_ID::Demon, spriteBuffer)->texture, size)) {
      editor->objectToPlaceId = ENTITY_ID::DEMON;
    }
    
    ImGui::SameLine();
    Sprite* golemSprite = GetSprite(SPRITE_ID::GolemRotate, spriteBuffer);
    ImVec2 golemUv1 = ImVec2(1.0f / golemSprite->spriteCountX, 1.0f / golemSprite->spriteCountY);
    if (ImGui::ImageButton("Golem", (ImTextureID)golemSprite->texture, size, uv0, golemUv1)) {   
      editor->objectToPlaceId = ENTITY_ID::GOLEM;
    }

    ImGui::SameLine();
    Sprite* medusaSprite = GetSprite(SPRITE_ID::MedusaRotate, spriteBuffer);
    ImVec2 medusaUv1 = ImVec2(1.0f / medusaSprite->spriteCountX, 1.0f / medusaSprite->spriteCountY);
    
    if (ImGui::ImageButton("Medusa", (ImTextureID)medusaSprite->texture, size, uv0, medusaUv1)) {  
      editor->objectToPlaceId = ENTITY_ID::MEDUSA;
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
        x -= 1;
        y -= 1;
        
        PlaceObject(x, y, editor, levelData, commandBuffer);
      }
    }
    else if (MousePressed(input, MouseButtons::RIGHT)) {
      if (CAMERA::GetIsPointInsideGrid(input->mouseX, input->mouseY, levelData)) {
        int x;
        int y;

        CAMERA::WorldToGrid(input->mouseX, input->mouseY, &x, &y, levelData);
        
        x -= 1;
        y -= 1;
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
      x -= 1;
      y -= 1;
    
      // Wrap the sprite in a SpriteRenderInfo and tell it to explicitly draw frame 0
      SpriteRenderInfo spriteInfo = {0, preview, false};
    
      RenderSpriteOnTile(spriteInfo, levelData, renderer, camera, x, y, 1, 0.5); 
    }
  }
}
