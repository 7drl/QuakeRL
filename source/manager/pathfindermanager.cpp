#include "pathfindermanager.h"
#include "map/pathfind/AStarPathfinder.h"

PathfinderManager::PathfinderManager()
	: pHeuristic(nullptr)
	, pPathfinder(nullptr)
{
}

PathfinderManager::~PathfinderManager()
{
	sdDelete(pPathfinder);
}

void PathfinderManager::Init(MapLayerTiled *mapBackground, MapLayerTiled *mapColliders)
{
	pHeuristic = sdNew(Heuristic);
	pPathfinder = sdNew(AStarPathfinder(true, true, 1, pHeuristic, mapBackground, mapColliders));
}


void PathfinderManager::Update(f32 dt)
{
	UNUSED(dt);
}

Path &PathfinderManager::Findpath(const Vector3f &start, const Vector3f &end, Path &path)
{
	return pPathfinder->FindPath(start, end, path);
}
