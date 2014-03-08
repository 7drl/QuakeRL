#include "entityfactory.h"

namespace
{
	typedef std::map<std::string, EntityFactory::CreateEntityProc> EntityCreatorMap;

	static EntityCreatorMap &GetCreatorMap()
	{
		static EntityCreatorMap mapCreators;
		return mapCreators;
	}
}

void EntityFactory::AddCreator(const char *name, CreateEntityProc proc)
{
	GetCreatorMap().insert(std::make_pair(name, proc));
}

Entity *EntityFactory::CreateEntity(const String &name)
{
	EntityCreatorMap &mapCreators = GetCreatorMap();

	EntityCreatorMap::iterator it = mapCreators.find(name);
	if (it == mapCreators.end())
	{
		Log("Entity %s not found.", name.c_str());
		return NULL;
	}

	return it->second();
}

Entity *EntityFactory::CreateEntity(const char *name)
{
	return CreateEntity(String(name));
}
