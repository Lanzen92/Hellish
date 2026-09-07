#pragma once

#include "entity.h"
#include <cstdint>
#include <winscard.h>


enum class CMD_TYPE : uint8_t {
  NONE = 0,
  MOVE = 1
};
 
struct Command {
  CMD_TYPE type;
  uint32_t timestamp;
};

struct MoveCommand : Command {
  Entity* entity;
  int xDir;
  int yDir;
};

union AnyCommand {
  Command command;
  MoveCommand move;

  AnyCommand(MoveCommand mv) {
    move = mv;  
  };
};

struct CommandBuffer {
  AnyCommand* allCommands;
  int capacity;
  int index;
  int head;
};

void Push (CommandBuffer* commandBuffer, AnyCommand command, uint32_t timestamp);
void Undo (CommandBuffer* commandBuffer);
void Redo (CommandBuffer* commandBuffer);


