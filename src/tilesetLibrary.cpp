#include <cassert>
#include "fstream"

#include "Parsers/json.hpp"

#include "tilesetLibrary.h"
#include "arena.h"

using namespace nlohmann;
using namespace std;

static const TilesetDataEntry AllTilesetData[] {
  { TILESETS::DUNGEON, "assets/tilesets/dungeon_tileset.tsj"}
};

uint16_t GetTilesetIDOffsetFromTilemap(int limitID, const json& tmjResult){
  int HighestTilemapStartId = 0;
  for (const json& tileset : tmjResult["tilesets"]) {
    int firstID = tileset["firstgid"].get<int>();
    if(firstID <= limitID && firstID > HighestTilemapStartId){
      HighestTilemapStartId = firstID;
    }
  }
  return HighestTilemapStartId;
}

uint16_t GetLocalTileID(uint16_t globalId, const json& tmjResult){
  return globalId - GetTilesetIDOffsetFromTilemap(globalId, tmjResult);
}

namespace AssetManagement {
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arenaImages){
    for (TilesetDataEntry entry : AllTilesetData) {
      LoadTileset(&entry, tilesetBuffer, arenaImages);
    }
  }

  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arenaImages){
    assert(entry->type != TILESETS::COUNT);
    assert(entry->type != TILESETS::NONE);
    
    Tileset* tileset = &tilesetBuffer[(int)entry->type];
    tileset->type = entry->type;
    fstream stream(entry->path);
    
    auto jsonResult = json::parse(stream);
    int tileCount = jsonResult["tilecount"].get<int>();
    
    tileset->walkableBuffer = ALLOC_ARRAY(arenaImages, bool, tileCount)
    auto& tiles = jsonResult["tiles"];
    for(const auto& tile : tiles){
      int tileId = tile["id"].get<int>();
      for(const auto& tileProperty : tile["properties"]){
        if(tileProperty["name"] == "walkable"){
          tileset->walkableBuffer[tileId] = tileProperty["value"].get<bool>();
        }
      }
    }
  }
}