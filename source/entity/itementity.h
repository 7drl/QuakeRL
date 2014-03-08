#ifndef _ITEM_ENTITY_H
#define _ITEM_ENTITY_H

#include "spriteentity.h"
#include "../util/collisionsensor.h"

class ItemEntity: public SpriteEntity
{
	public:
		ItemEntity(const char *className, const char *spriteName);
		virtual ~ItemEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		void Update(f32 dt);

		virtual void OnCollision(const CollisionEvent &event);

	protected:
		u32 iAmount;
		CollisionSensor clSensor;
};

#endif
