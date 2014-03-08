#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "../defines.h"

class Entity;
typedef std::vector<Entity *> EntitiesVector;

class WorldManager
{
	public:
		virtual ~WorldManager();

		Entity *BuildEntity(MetadataObject &metadata, SceneNode *sprites);

		void Clear();
		void Update(f32 dt);

		Entity *FindEntityByClassName(const String &className);
		Entity *FindEntityByName(const String &name);
		void ActivateAllEntites(const String &name);

	private:
		EntitiesVector vEntities;
};

#endif
