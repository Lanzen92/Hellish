
#include <cstring>
#include <windows.h>
#include "arena.h"
using namespace std;

//Init arena, med storlek av size-inparam. void* = tom minnesplats.
void Memory::Initialize(Arena* arena, void* start, size_t size) {
  arena->base = (unsigned char*) start;
  arena->size = size;
  arena->used = 0;
}

//Allocate minne. Nollar minnet, allokerar den angivna mangden och retunerar forsta minnesadressen i det nya blocket.
void* Memory::Allocate(Arena* arena, size_t size) {
  if (arena->used + size > arena->size) {
    return nullptr;
  }

  void* front = arena->base + arena->used;
  arena->used += size;
  memset(front, 0, size);
  return front;
}

//Sätt used till 0, för att reseta.
void Memory::Reset(Arena* arena) {
  arena->used = 0;
}

Memory::Arena* Memory::CreateSubArena(Arena* parentArena, size_t size) {
  Memory::Arena* subArena = (Memory::Arena*)Allocate(parentArena, sizeof(Memory::Arena));

  void* memoryStart = Allocate(parentArena, size);
  Memory::Initialize(subArena, memoryStart, size);

  return subArena;
}

struct Character {
  enum CHARACTER_TYPE {HERO, ENEMY};
  CHARACTER_TYPE my_character_type;
  int health;
  int damage;
  bool is_alive;
  char* name;  
};

