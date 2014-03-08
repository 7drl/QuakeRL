#ifndef _GOLD_ENTITY_H
#define _GOLD_ENTITY_H

#include "itementity.h"

class GoldEntity: public ItemEntity
{
	public:
		GoldEntity();
		void OnCollision(const CollisionEvent &event);
};

#endif // _GOLD_ENTITY_H
