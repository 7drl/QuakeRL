#ifndef _PROJECTILE_ENTITY_H
#define _PROJECTILE_ENTITY_H

#include "spriteentity.h"
#include "../util/collisionsensor.h"

class ProjectileEntity: public SpriteEntity
{
	public:
		ProjectileEntity();
		virtual ~ProjectileEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void Update(f32 dt);

		Vector3f GetPosition();
		Sprite *GetSprite() const;

		//virtual void SetTarget(SpriteEntity *target);
		virtual void OnCollision(const CollisionEvent &event);
		virtual void Activate();

	private:

		SpriteEntity *pTarget;
		CollisionSensor clSensor;
		f32 fSleepConfig;
		f32 fSleep;
		f32 fSpeedFactor;

};

#endif // _PROJECTILE_ENTITY_H
