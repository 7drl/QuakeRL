#include "pathfindermanager.h"
#include "map/pathfind/AStarPathfinder.h"

PathfinderManager::PathfinderManager()
	: pPathfinder(nullptr)
{
	pPathfinder = sdNew(AStarPathfinder);
}

PathfinderManager::~PathfinderManager()
{
	sdDelete(pPathfinder);
}

Path &PathfinderManager::Findpath(const Vector3f &start, const Vector3f &end, MapLayerTiled &map, Path &path)
{
	return pPathfinder->FindPath(start, end, map, path);
}

void PathfinderManager::Update(f32 dt)
{
	UNUSED(dt);
}
