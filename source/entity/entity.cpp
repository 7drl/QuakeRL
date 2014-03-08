#include "entity.h"
#include "../scene/gamescene.h"

Entity::Entity(const String &className)
	: strClassName(className)
{
}

Entity::~Entity()
{
}

void Entity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	UNUSED(sprites)
	strName = metadata.sName;
	strTarget = metadata.GetProperty("Target");
}

void Entity::DoActivateAll()
{
	if (!strTarget.empty())
		gWorldManager->ActivateAllEntites(strTarget.c_str());
}
