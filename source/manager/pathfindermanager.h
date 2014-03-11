#ifndef __PATHFINDERMANAGER_H__
#define __PATHFINDERMANAGER_H__

#include "../defines.h"
#include "map/pathfind/IPathfinder.h"

using namespace Seed;

class PathfinderManager
{
	SEED_DISABLE_COPY(PathfinderManager)
	SEED_DECLARE_CONTAINER(Vector, Path)

	public:
		PathfinderManager();
		virtual ~PathfinderManager();

		Path &Findpath(const Vector3f &start, const Vector3f &end, MapLayerTiled &map, Path &path);
		void Update(f32 dt);

	private:
		IPathfinder *pPathfinder;

};

#endif // __PATHFINDERMANAGER_H__
