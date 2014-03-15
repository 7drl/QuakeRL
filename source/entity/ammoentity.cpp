#include "ammoentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("Ammo", AmmoEntity);

AmmoEntity::AmmoEntity()
	: ItemEntity("Ammo", "Ammo")
{

}

AmmoEntity::~AmmoEntity()
{

}

void AmmoEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	pSprite->SetZ(-10);

	pSprite->SetAnimation(metadata.GetProperty("AnimationName"));

	clSensor.Load(metadata, this);

	if (!metadata.GetProperty("Amount").empty())
		iAmount = std::stoi(metadata.GetProperty("Amount"));
	else
		iAmount = 1;
}

void AmmoEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Log("On collided with weapon");

		Entity *other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "OptimistPlayer")
		{
			PlayerEntity *player = static_cast<PlayerEntity *>(other);

			// Disable item
			this->pSprite->SetVisible(false);
			this->clSensor.Disable();

			//Collect Item
			player->OnCollect(ItemTypes::HealthPotion, this->iAmount);

		}
	}
}