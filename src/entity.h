#pragma once

#include <cstdint>
#include <cassert>


enum Behaviour: uint32_t {
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2  
};

enum class ID : uint8_t {
  NONE = 0,
  WALL = 1,
  GROUND = 2,
  DEMON = 3,
  ROCK = 4,
  MEDUSA = 5,
  GHOST = 6,
  GOLEM = 7
};

struct Position {
  int x;
  int y;
};

struct Entity {
  ID id;
  int x;
  int y;
  int xPrev;
  int yPrev;
  float progress01;
  Behaviour behaviour;

  bool HasBehaviour(Behaviour flags) {
    return (behaviour & flags) == flags;
  }

  void SetBehaviour(Behaviour flags) {
    behaviour = flags;
  }

  void AddBehaviour(Behaviour flags) {
    behaviour = (Behaviour(behaviour | flags));
  }

  void RemoveBehaviour(Behaviour flags) {
    behaviour = (Behaviour)(behaviour & -flags);
  }

  void InitializeBaseBehaviour() {
    assert(id != ID::NONE);
    switch(id) {
      default:
        SetBehaviour(NONE);
        break;
      case ID::DEMON:
        SetBehaviour((Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
        break;
      case ID::ROCK:
        SetBehaviour((Behaviour)CAN_MOVE);
    }
  }
};

bool IsMoving(Entity* entity);
