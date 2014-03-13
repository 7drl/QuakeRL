#ifndef _AMMO_ENTITY_H
#define _AMMO_ENTITY_H

#include "itementity.h"

class AmmoEntity: public ItemEntity
{
	public:
		AmmoEntity();
		virtual ~AmmoEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void OnCollision(const CollisionEvent &event);

};

#endif // _AMMO_ENTITY_H
