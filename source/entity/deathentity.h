#ifndef _DEATHENTITY_H
#define _DEATHENTITY_H

#include "spriteentity.h"
#include "../util/collisionsensor.h"

class PlayerEntity;

class DeathEntity: public SpriteEntity
{
	public:
		DeathEntity();
		virtual ~DeathEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void Update(f32 dt);

		Vector3f GetPosition();
		Sprite *GetSprite() const;

		virtual void OnCollision(const CollisionEvent &event);
		virtual void Activate();

	private:
		PlayerEntity *pTarget;
		CollisionSensor clSensor;
		f32 fSleepConfig;
		f32 fSleep;
		f32 fSpeedFactor;

};

#endif
