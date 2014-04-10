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

Path &PathfinderManager::Findpath(const vec3 &start, const vec3 &end, Path &path)
{
	return pPathfinder->FindPath(start, end, path);
}
