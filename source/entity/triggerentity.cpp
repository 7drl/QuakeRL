#include "triggerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("Trigger", TriggerEntity)

TriggerEntity::TriggerEntity()
	: Entity("Trigger")
	, bOnce(true)
	, iCount(0)
{
}

TriggerEntity::~TriggerEntity()
{
}

void TriggerEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	Entity::Load(metadata, sprites);
	clSensor.Load(metadata, this);

	if (!metadata.GetProperty("Once").empty())
		bOnce = metadata.GetProperty("Once").compare("true") == 0;
}

void TriggerEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		if (event.GetOtherEntity() && event.GetOtherEntity()->GetClassName().compare("Player") == 0)
		{
			Log("Trigger colidiu");

			if (bOnce && iCount > 0)
			{
				Log("Ignoring, once");
				return;
			}

			++iCount;

			this->DoActivateAll();
		}
	}
}
