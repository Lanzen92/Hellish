#pragma once

namespace Memory {

  struct Arena {
    unsigned char* base;
    size_t size;
    size_t used;
  };

  void Initialize(Arena *arena, void *start, size_t size);
  void* Allocate(Arena* arena, size_t size);
  void Reset(Arena* arena);
  Arena* CreateSubArena(Arena* parentArena, size_t size);


}
