#ifndef __PATHFINDERMANAGER_H__
#define __PATHFINDERMANAGER_H__

#include "../defines.h"
#include <map/pathfind/IPathfinder.h>
#include <map/pathfind/Heuristic.h>

using namespace Seed;

class PathfinderManager
{
	SEED_DISABLE_COPY(PathfinderManager)
	SEED_DECLARE_CONTAINER(Vector, Path)

	public:
		PathfinderManager();
		virtual ~PathfinderManager();

		void Init(MapLayerTiled *mapBackground);
		void Update(f32 dt);
		Path &Findpath(const vec3 &start, const vec3 &end, Path &path);

	private:
		IPathfinder *pPathfinder;
};

#endif // __PATHFINDERMANAGER_H__
