#include "healthpotionentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("HealthPotion", HealthPotionEntity)

HealthPotionEntity::HealthPotionEntity()
	: ItemEntity("HealthPotion", "HealthPotion")
{
}

void HealthPotionEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Log("On collided with health potion");

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
