#include "pathfindermanager.h"
#include "map/pathfind/AStarPathfinder.h"
#include "proceduralmanager.h"

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
	pPathfinder = sdNew(AStarPathfinder(false, false, 40, ProceduralManager::eTiles::tileStoneWall, mapBackground));
}


void PathfinderManager::Update(f32 dt)
{
	UNUSED(dt);
}

Path &PathfinderManager::Findpath(const Vector3f &start, const Vector3f &end, Path &path)
{
	return pPathfinder->FindPath(start, end, path);
}
