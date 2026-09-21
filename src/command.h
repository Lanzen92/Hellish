#pragma once

#include <cstdint>

#include "entity.h"

enum class CMD_TYPE : uint8_t {
  NONE = 0,
  MOVE = 1,
  ROTATE = 2,
  MODIFY_BEHAVIOUR = 3,
  ADD = 4,
  REMOVE = 5
};
 
struct Command {
  CMD_TYPE type = CMD_TYPE::NONE;
  uint32_t timestamp;
};

struct AddCommand : Command {
  ID id;
  int x;
  int y;

  AddCommand(ID id, int x, int y) {
    this->type = CMD_TYPE::ADD;
    this->id = id;
    this->x = x;
    this->y = y;
  };
};

struct RemoveCommand : Command {
  ID storedId;
  Behaviour storedBehaviour;
  int x;
  int y;

  RemoveCommand(Entity* entity) {
    this->type = CMD_TYPE::REMOVE;
    x = entity->x;
    y = entity->y;
    storedId = entity->id;
    storedBehaviour = entity->behaviour;
  };
};

struct MoveCommand : Command {
  Entity* entity;
  int xDir;
  int yDir;

  MoveCommand(Entity* entity, int xDir, int yDir) {
    this->type = CMD_TYPE::MOVE;
    this->entity = entity;
    this->xDir = xDir;
    this->yDir = yDir;
  };
};

struct RotateCommand : Command {
  Entity* entity;
  Direction from;
  Direction to;

  RotateCommand(Entity* entity, Direction from, Direction to) {
    this->type = CMD_TYPE::ROTATE;
    this->entity = entity;
    this->from = from;
    this->to = to;
  };
};

struct ModifyBehaviourCommand : Command {
  enum Mode {
    ADD,
    REMOVE
  };
  
  Entity* entity;
  Behaviour flag;
  Mode mode;

  ModifyBehaviourCommand(Entity* entity, Behaviour flag, Mode mode) {
    this->type = CMD_TYPE::MODIFY_BEHAVIOUR;
    this->entity = entity;
    this->flag = flag;
    this->mode = mode;
  };
};

union AnyCommand {
  Command command;
  MoveCommand move;
  RotateCommand rotate;
  ModifyBehaviourCommand modify;
  AddCommand add;
  RemoveCommand remove;

  AnyCommand(MoveCommand mv) { move = mv; };
  AnyCommand(RotateCommand rt) { rotate = rt; };
  AnyCommand(ModifyBehaviourCommand mb) { modify = mb; };
  AnyCommand(AddCommand ad) { this->add = ad; };
  AnyCommand(RemoveCommand rm) { remove = rm; };
};

struct CommandBuffer {
  AnyCommand* allCommands;
  int capacity;
  int index;
  int head;
  uint32_t timestamp;
};

void Push(CommandBuffer* commandBuffer, AnyCommand command, LevelData* levelData);
void Undo(CommandBuffer* commandBuffer, LevelData* levelData);
void Redo(CommandBuffer* commandBuffer, LevelData* levelData);
