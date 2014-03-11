#ifndef _ENEMY_ENTITY_H
#define _ENEMY_ENTITY_H

#include "spriteentity.h"
#include "../util/collisionsensor.h"
#include "playerentity.h"

class EnemyEntity: public SpriteEntity
{
	public:
		EnemyEntity();
		virtual ~EnemyEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		void Update(f32 dt);

		virtual void OnCollision(const CollisionEvent &event);

		bool OnDamage(u32 amount);

		struct EnemyData
		{
			String displayName;
			u32 iEnemyId;
			u32 iLevel;
			u32 iAttackPower;
			u32 iDefensePower;
			u32 iLife;
			u32 iLifeTotal;
		} sEnemy;

		String GetDisplayName() const;
		void SetDisplayName(const String &displayName);

		u32 GetLevel() const;
		void SetLevel(u32 level);

		u32 GetLife() const;
		void SetLife(u32 life);

	private:
		PlayerEntity *pTarget;
		b2Body *pBody;
		CollisionSensor clSensor;
		f32 fInvicibleTime;
		bool bPlayerLock;
		bool bIsDead;
};

#endif
