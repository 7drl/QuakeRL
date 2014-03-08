#ifndef _STAMINA_POTION_ENTITY_H
#define _STAMINA_POTION_ENTITY_H

#include "itementity.h"

class StaminaPotionEntity: public ItemEntity
{
	public:
		StaminaPotionEntity();
		void OnCollision(const CollisionEvent &event);
};

#endif // _STAMINA_POTION_ENTITY_H
