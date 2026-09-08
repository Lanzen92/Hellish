
#include "entity.h"


bool IsMoving(Entity* entity) {
  return entity->x != entity->xPrev || entity->y != entity->yPrev;
}
