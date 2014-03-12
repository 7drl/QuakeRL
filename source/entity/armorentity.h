#ifndef _ARMOR_ENTITY_H
#define _ARMOR_ENTITY_H

#include "itementity.h"

class ArmorEntity: public ItemEntity
{
	public:
		ArmorEntity();
		virtual ~ArmorEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void OnCollision(const CollisionEvent &event);
};

#endif // ARMORENTITY_H
