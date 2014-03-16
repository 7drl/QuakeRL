#ifndef _WEAPON_ENTITY_H
#define _WEAPON_ENTITY_H

#include "itementity.h"

class WeaponEntity: public ItemEntity
{
	public:
		WeaponEntity();
		virtual ~WeaponEntity();

		virtual void Load(MetadataObject &metadata, SceneNode *sprites);
		virtual void OnCollision(const CollisionEvent &event);

	private:
		u32 uWeaponId;
};

#endif // _WEAPON_ENTITY_H
