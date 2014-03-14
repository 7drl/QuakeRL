#include "pathfindermanager.h"
#include "map/pathfind/AStarPathfinder.h"

PathfinderManager::PathfinderManager()
	: pPathfinder(nullptr)
{
}

PathfinderManager::~PathfinderManager()
{
	sdDelete(pPathfinder);
}

void PathfinderManager::Init(MapLayerTiled *mapBackground)
{
	pPathfinder = sdNew(AStarPathfinder(true, true, 40, 3, mapBackground));
}


void PathfinderManager::Update(f32 dt)
{
	UNUSED(dt);
}

Path &PathfinderManager::Findpath(const Vector3f &start, const Vector3f &end, Path &path)
{
	return pPathfinder->FindPath(start, end, path);
}
