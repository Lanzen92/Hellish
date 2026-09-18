#pragma once

#include <cstdint>
#include <cassert>

struct CommandBuffer;
struct LevelData;

enum Behaviour: uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2, 
  IS_PETRIFIED = 1 << 3,
  CAN_ROTATE = 1 << 4,
  UNPUSHABLE = 1 << 5,
  JUMPS = 1 << 6,
  IS_PUSHING = 1 << 7,
};

enum class ID : uint8_t {
  NONE = 0,
  WALL = 1,
  GROUND = 2,
  DEMON = 3,
  ROCK = 4,
  MEDUSA = 5,
  GHOST = 6,
  GOLEM = 7,
  SIREN = 8
};

enum class Direction {
  RIGHT,
  LEFT,
  UP,
  DOWN
};

struct Position {
  int x;
  int y;
};

struct Entity {
  ID id;
  Direction facing;
  int strength;
  int x;
  int y;
  int xPrev;
  int yPrev;
  float progress01;
  Behaviour behaviour;


};

bool HasBehaviour(Entity* entity, Behaviour flags);

void SetBehaviour(Entity* entity, Behaviour flags);

void AddBehaviour(Entity* entity, Behaviour flags);

void RemoveBehaviour(Entity* entity, Behaviour flags);

void InitializeBaseBehaviour(Entity* entity);

bool IsMoving(Entity* entity);

void PostMove(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer);

void PostRotation(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer, Direction from, Direction to);

void PreRotation(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer, Direction from, Direction to);

inline Direction DirectionFromXY(int xDir, int yDir) { 
  assert(xDir * yDir == 0);
  if (xDir == 1) { 
    return Direction::RIGHT; 
  }
  if (xDir == -1) {
    return Direction::LEFT;
  }
  if (yDir == 1) {
    return Direction::UP;
  } 
  else {
    return Direction::DOWN;
  }
}

