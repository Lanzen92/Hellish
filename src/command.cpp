#include <cstdint>
#include "command.h"
#include "level.h"
#include "entity.h"

void Execute(AnyCommand cmd, LevelData* levelData, CommandBuffer* commandBuffer, bool fromRedo = false) {
  switch(cmd.command.type) {
    case CMD_TYPE::NONE:
      break;

    case CMD_TYPE::MOVE: {
      MoveCommand mc = cmd.move;
      mc.entity->xPrev = mc.entity->x;
      mc.entity->yPrev = mc.entity->y;
      mc.entity->x += mc.xDir;
      mc.entity->y += mc.yDir;

      if (fromRedo) {
        mc.entity->progress01 = 1;
      }

      PostMove(mc.entity, levelData, commandBuffer);
    } 
    break;

    case CMD_TYPE::ROTATE: {

      RotateCommand rc = cmd.rotate;
      if (!HasBehaviour(rc.entity, CAN_ROTATE)) {
        break;
      }

      PreRotation(rc.entity, levelData, commandBuffer, rc.from, rc.to);
      rc.entity->facing = rc.to;
      PostRotation(rc.entity, levelData, commandBuffer, rc.from, rc.to);
    } 
    break;

    case CMD_TYPE::MODIFY_BEHAVIOUR: {
      ModifyBehaviourCommand mb = cmd.modify;
      if (mb.mode == ModifyBehaviourCommand::Mode::ADD) {
        AddBehaviour(mb.entity, mb.flag);
      } 
      else {
        RemoveBehaviour(mb.entity, mb.flag);
      }
    }
    break;

  }
}

void Push(CommandBuffer* commandBuffer, AnyCommand cmd, LevelData* levelData) {
  assert(cmd.command.type != CMD_TYPE::NONE);

  commandBuffer->allCommands[commandBuffer->index] = cmd;
  commandBuffer->allCommands[commandBuffer->index].command.timestamp = commandBuffer->timestamp;
  commandBuffer->index++;
  commandBuffer->head = commandBuffer->index;
  Execute(cmd, levelData, commandBuffer);
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
    case CMD_TYPE::MOVE: {   
      MoveCommand mc = cmd.move;
      mc.entity->x -= mc.xDir;
      mc.entity->y -= mc.yDir;
      mc.entity->progress01 = 1;
      break;
    }
    
    case CMD_TYPE::ROTATE: {
      RotateCommand rc = cmd.rotate;
      if (!HasBehaviour(rc.entity, CAN_ROTATE)) {
        break;
      }
      rc.entity->facing = rc.from;
      break;
    }

    case CMD_TYPE::MODIFY_BEHAVIOUR: {
      ModifyBehaviourCommand mb = cmd.modify;
      if (mb.mode == ModifyBehaviourCommand::Mode::ADD) {
        RemoveBehaviour(mb.entity, mb.flag);
      } 
      else {
        AddBehaviour(mb.entity, mb.flag);
      }

      break;
    }
  }

  if (commandBuffer->index > 0) {
    if (commandBuffer->allCommands[commandBuffer->index - 1].command.timestamp == timestamp) {
      Undo(commandBuffer);
    }
  }
}

void Redo(CommandBuffer* commandBuffer, LevelData* levelData){
  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];

  if(cmd.command.type == CMD_TYPE::NONE){
    return;
  }
  
  if(commandBuffer->index == commandBuffer->head){
    return;
  }
  
  commandBuffer->index++;
  Execute(cmd, levelData, commandBuffer, true);

  uint32_t timestamp = cmd.command.timestamp;
  if (commandBuffer->index != commandBuffer->head) {
    AnyCommand nextCommand = commandBuffer->allCommands[commandBuffer->index];
    
    if (nextCommand.command.timestamp == timestamp) {
      Redo(commandBuffer, levelData);
    }
  }
}
