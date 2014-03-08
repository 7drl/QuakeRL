#ifndef _HEALTH_POTION_ENTITY_H
#define _HEALTH_POTION_ENTITY_H

#include "itementity.h"

class HealthPotionEntity: public ItemEntity
{
	public:
		HealthPotionEntity();
		void OnCollision(const CollisionEvent &event);
};

#endif // _HEALTH_POTION_ENTITY_H
