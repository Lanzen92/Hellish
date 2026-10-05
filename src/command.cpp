#include <cstdint>

#include "SDL3_image/SDL_image.h" //SDL Log
#include "command.h"

#include "level.h"
#include "entity.h"

void Execute(AnyCommand cmd, LevelData* levelData, CommandBuffer* commandBuffer, bool fromRedo = false) {

  SDL_Log("Execute called with command type: %d (NONE is usually 0)", (int)cmd.command.type);

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
      
      mc.entity->action = Actions::MOVING;
      if(!fromRedo) {
        PostMove(mc.entity, levelData, commandBuffer);
      }

      break;
    }
    case CMD_TYPE::ROTATE: {

      RotateCommand* rc = &cmd.rotate;
      if (!HasBehaviour(rc->entity, CAN_ROTATE)) {
        break;
      }

      if(fromRedo) {
        rc->entity->progress01 = 1;
      }
      rc->entity->action = Actions::ROTATING;
      
      if(!fromRedo) {
        PreRotation(rc->entity, levelData, commandBuffer, rc->from, rc->to);
      }
      
      rc->entity->facingPrevious = rc->from;
      rc->entity->facingCurrent = rc->to;
      
      if(!fromRedo) {
        PostRotation(rc->entity, levelData, commandBuffer, rc->from, rc->to);
      }
      break;
    } 
    case CMD_TYPE::MODIFY_BEHAVIOUR: {
      ModifyBehaviourCommand mb = cmd.modify;
      if (mb.mode == ModifyBehaviourCommand::Mode::ADD) {
        AddBehaviour(mb.entity, mb.flag);
      } 
      else {
        RemoveBehaviour(mb.entity, mb.flag);
      }
      break;
    }
    case CMD_TYPE::ADD: {
      AddCommand* ac = &cmd.add;
      AddEntity(ac->id, ac->x, ac->y, levelData);
      break;
    }
    case CMD_TYPE::REMOVE: {
      RemoveCommand* remove = &cmd.remove;
      RemoveEntity(remove->x, remove->y, levelData);
      break;
    }
    case CMD_TYPE::SWAP_ACTIVE: {
      SwapActiveEntityCommand* swap = &cmd.swap;
      *swap->valueToChange = swap->indexCurrent;
      break;
    }
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

void Undo(CommandBuffer* commandBuffer, LevelData* levelData) {
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
      RotateCommand rotate = cmd.rotate;
      if(!HasBehaviour(rotate.entity, CAN_ROTATE)){
        break;
      }
      rotate.entity->facingCurrent = rotate.from;
      rotate.entity->progress01 = 1;
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
    case CMD_TYPE::ADD: {
      AddCommand* ac = &cmd.add;
      RemoveEntity(ac->x, ac->y, levelData);
      break;
    }
    case CMD_TYPE::REMOVE: {
      RemoveCommand* remove = &cmd.remove;
      AddEntity(remove->storedId, remove->x, remove->y, levelData);
      Entity* entity = GetEntity(levelData, remove->x, remove->y);
      SetBehaviour(entity, remove->storedBehaviour);
      break;
    }
    case CMD_TYPE::SWAP_ACTIVE:{
      SwapActiveEntityCommand* swap = &cmd.swap;
      *swap->valueToChange = swap->indexPrevious;
      break;
    }
  }

  if (commandBuffer->index > 0) {
    if (commandBuffer->allCommands[commandBuffer->index - 1].command.timestamp == timestamp) {
      Undo(commandBuffer, levelData);
    }
  }
}

void Redo(CommandBuffer* commandBuffer, LevelData* levelData) {
  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];

  if (cmd.command.type == CMD_TYPE::NONE) {
    return;
  }

  if (commandBuffer->index == commandBuffer->head) {
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

void ResetCommandBuffer(CommandBuffer* commandBuffer) {
  commandBuffer->index = 0;
  commandBuffer->head = 0;
  commandBuffer->timestamp = 0;
}
