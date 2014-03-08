#ifndef _TRIGGER_COUNTER_H
#define _TRIGGER_COUNTER_H

#include "../entity/entity.h"

class TriggerCounter: public Entity
{
	public:
		TriggerCounter();
		virtual ~TriggerCounter();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void Activate();

	private:
		s32 iCounter;
};

#endif
