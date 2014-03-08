#ifndef _COLLISION_SENSOR_H
#define _COLLISION_SENSOR_H

#include "../defines.h"
#include <Box2D/Box2D.h>

class CollisionSensor
{
	public:
		CollisionSensor();
		~CollisionSensor();

		void Load(ISceneObject &metadata, void *userData);
		void Load(ISceneObject &metadata, bool track, b2Vec2 *customSize, void *userData);

		void Disable();

		inline b2Vec2 GetBodyPosition() const
		{
			return pBody->GetPosition();
		}

		inline void SetBodyPosition(const b2Vec2 &position)
		{
			pBody->SetTransform(position, pBody->GetAngle());
		}

	private:
		b2Body *pBody;
};

#endif
