#ifndef _PROCEDURAL_MANAGER_H
#define _PROCEDURAL_MANAGER_H

#include "../defines.h"

class ProceduralManager
{
	public:
		ProceduralManager();
		virtual ~ProceduralManager();
		virtual void BuildWorld(const int width, const int height, int dungeonObjects);
		virtual void SetTile(int x, int y, int tileType);
		virtual int GetTile(int x, int y);
		virtual int GetXSize();
		virtual int GetYSize();

		enum
		{
			tileUnused = 0,
			tileBrickFloor,
			tileGrassFloor,
			tileStoneWall,
			tileMetalFloor,
			tileCorridor,
			tileDoor,
			tileUpStairs,
			tileDownStairs,
			tileChest
		};

	private:

		virtual int GetRand(int min, int max);
		virtual bool MakeRoom(int x, int y, int xlength, int ylength, int direction);
		virtual bool MakeCorridor(int x, int y, int lenght, int direction);

		int *pWorldMap;

		int iXMax;
		int iYMax;

		int iXSize;
		int iYSize;

		long lOldSeed;

		int iObjects;
		int iChanceRoom;
		int iChanceCorridor;

};

#endif // _PROCEDURAL_MANAGER_H
