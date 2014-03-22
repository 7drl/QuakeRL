#include "weaponentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("Weapon", WeaponEntity)

WeaponEntity::WeaponEntity()
	: ItemEntity("Weapon", "Weapon")
	, uWeaponId(0)
{
}

WeaponEntity::~WeaponEntity()
{
}

void WeaponEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	pSprite->SetZ(-10);

	pSprite->SetAnimation(metadata.GetProperty("AnimationName"));

	clSensor.Load(metadata, this);

	uWeaponId = std::stoi(metadata.GetProperty("id"));

	if (!metadata.GetProperty("Amount").empty())
		iAmount = std::stoi(metadata.GetProperty("Amount"));
	else
		iAmount = 1;

}

void WeaponEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Log("On collided with weapon");

		auto other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "OptimistPlayer")
		{
			auto player = static_cast<PlayerEntity *>(other);

			// Disable item
			this->pSprite->SetVisible(false);
			this->clSensor.Disable();

			//Collect Item
			player->OnCollect(uWeaponId, this->iAmount);
		}
	}
}
