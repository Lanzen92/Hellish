#pragma once

#include "Parsers/json.hpp"

using namespace nlohmann;

namespace Memory {
struct Arena;
}

enum class TILESETS {
  NONE = 0,
  DUNGEON = 1,
  COUNT = 2
};

struct Tileset {
  TILESETS type;
  bool* walkableBuffer;
};

struct TilesetDataEntry {
  TILESETS type;
  const char* path;
};

uint16_t GetLocalTileID(uint16_t globalId, const json& tmjResult);
uint16_t GetTilesetIDOffsetFromTilemap(int limitId, const json& tmjResult);

namespace AssetManagement {
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arenaImages);
  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arenaImages);
}