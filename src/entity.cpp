#include "entity.h"
#include "level.h"
#include "command.h"

bool IsMoving(Entity* entity) {
  return entity->x != entity->xPrev || entity->y != entity->yPrev;
}

bool HasBehaviour(Entity* entity, Behaviour flags) {
  return (entity->behaviour & flags) == flags;
}

void InitializeBaseBehaviour(Entity* entity) {
  assert(entity->id != ID::NONE);
  switch (entity->id) {
  default:
    SetBehaviour(entity, NONE);
    break;
  case ID::DEMON:
    SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
    entity->strength = 1;
    break;
  case ID::GOLEM:
    SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
    AddBehaviour(entity, UNPUSHABLE);
    entity->strength = 999;
    break;
  case ID::MEDUSA:
    SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
    entity->strength = 1;
    break;
  case ID::SIREN:
    SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
    entity->strength = 0;
    break;
  case ID::ROCK:
    SetBehaviour(entity, (Behaviour)CAN_MOVE);
    break;
  }
}

void SetBehaviour(Entity* entity, Behaviour flags) {
  entity->behaviour = flags;
}

void AddBehaviour(Entity* entity, Behaviour flags) {
  entity->behaviour = (Behaviour)(entity->behaviour | flags);
}

void RemoveBehaviour(Entity* entity, Behaviour flags) {
  entity->behaviour = (Behaviour)(entity->behaviour & ~flags);
}

void PostMove(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer) {
  if (entity->id == ID::MEDUSA) {
    Entity* entityLookedAt = RaycastFirstEntity(entity->x, entity->y, entity->facing, levelData);
    if (entityLookedAt != nullptr) {
      if (!HasBehaviour(entityLookedAt, Behaviour::IS_PETRIFIED)) {
        AddBehaviour(entityLookedAt, Behaviour::IS_PETRIFIED);
      }
    }
  }
}

void PostRotation(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer, Direction from, Direction to) {
  if (from == to) {
    return;
  }
  if (entity->id == ID::MEDUSA) {
    Entity* entityLookedAt = RaycastFirstEntity(entity->x, entity->y, to, levelData);
    if (entityLookedAt != nullptr) {
      if (!HasBehaviour(entityLookedAt, Behaviour::IS_PETRIFIED)) {
        ModifyBehaviourCommand modify(entityLookedAt, Behaviour::IS_PETRIFIED, ModifyBehaviourCommand::Mode::ADD);
        Push(commandBuffer, modify, levelData);
      }
    }
  }
}

void PreRotation(Entity* entity, LevelData* levelData, CommandBuffer* commandBuffer, Direction from, Direction to) {
  if (from == to) {
    return;
  }
  if (entity->id == ID::MEDUSA) {
    Entity* entityPrevLookedAt = RaycastFirstEntity(entity->x, entity->y, from, levelData);
    if (entityPrevLookedAt != nullptr) {
      if (HasBehaviour(entityPrevLookedAt, Behaviour::IS_PETRIFIED)) {
        RemoveBehaviour(entityPrevLookedAt, Behaviour::IS_PETRIFIED);
      }
    }
  }
}