
#include "command.h"
#include <cstdint>

void Execute(AnyCommand cmd) {
  switch(cmd.command.type) {
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
      mv.entity->x += mv.xDir;
      mv.entity->y += mv.yDir;
      break;
  }
}

void Push (CommandBuffer* commandBuffer, AnyCommand cmd, uint32_t timestamp) {
  commandBuffer->allCommands[commandBuffer->index] = cmd;
  commandBuffer->allCommands[commandBuffer->index].command.timestamp = timestamp;
  commandBuffer->index++;
  commandBuffer->head = commandBuffer->index;
  Execute(cmd);
}

void Undo(CommandBuffer* commandBuffer) {
  if (commandBuffer->index == 0) {
    return;
  }
  commandBuffer->index--;

  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];
  uint32_t timestamp = cmd.command.timestamp;

  switch (cmd.command.type) {
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;
      mv.entity->y -= mv.yDir;
      break;
  }

  if (commandBuffer->index > 0) {
    if (commandBuffer->allCommands[commandBuffer->index - 1].command.timestamp == timestamp) {
      Undo(commandBuffer);
    }
  }
  
}
void Redo(CommandBuffer* commandBuffer){
  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];

  if(cmd.command.type == CMD_TYPE::NONE){
    return;
  }
  
  if(commandBuffer->index == commandBuffer->head){
    return;
  }
  
  commandBuffer->index++;
  Execute(cmd);

  uint32_t timestamp = cmd.command.timestamp;
  if (commandBuffer->index != commandBuffer->head) {
    AnyCommand nextCommand = commandBuffer->allCommands[commandBuffer->index];
    
    if (nextCommand.command.timestamp == timestamp) {
      Redo(commandBuffer);
    }
  }
}
