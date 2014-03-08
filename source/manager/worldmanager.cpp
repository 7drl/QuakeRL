#include "worldmanager.h"
#include "../entity/entityfactory.h"
#include "../entity/entity.h"

WorldManager::~WorldManager()
{
	this->Clear();
}

Entity *WorldManager::BuildEntity(MetadataObject &metadata, SceneNode *sprites)
{
	const String &className = metadata.GetProperty("Class");

	Log("%s", className.c_str());

	Entity *ent = EntityFactory::CreateEntity(className);
	if (ent == NULL)
		return NULL;

	ent->Load(metadata, sprites);
	vEntities.push_back(ent);

	return ent;
}

void WorldManager::Clear()
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		sdDelete(*it);
	}

	vEntities.clear();
}

void WorldManager::Update(f32 dt)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		(*it)->Update(dt);
	}
}

Entity *WorldManager::FindEntityByClassName(const String &className)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetClassName().compare(className) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

Entity *WorldManager::FindEntityByName(const String &name)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetName().compare(name) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

void WorldManager::ActivateAllEntites(const String &name)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetName().compare(name) == 0)
		{
			(*it)->Activate();
		}
	}
}

