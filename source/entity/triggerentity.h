#ifndef _TRIGGER_ENTITY_H
#define _TRIGGER_ENTITY_H

#include "entity.h"
#include "../util/collisionsensor.h"

class TriggerEntity: public Entity
{
	public:
		TriggerEntity();
		virtual ~TriggerEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void OnCollision(const CollisionEvent &event);

	private:
		bool bOnce;
		s32 iCount;

		CollisionSensor clSensor;
};

#endif
