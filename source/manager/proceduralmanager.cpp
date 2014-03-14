#include "proceduralmanager.h"
#include "Memory.h"

ProceduralManager::ProceduralManager()
	: pWorldMap(nullptr)
	, pEnemiesMap(nullptr)
	, pItemsMap(nullptr)
	, iXMax(30)
	, iYMax(30)
	, iXSize(0)
	, iYSize(0)
	, iObjects(0)
	, iChanceRoom(0)
	, iChanceCorridor(0)
	, clRand()
{

}

ProceduralManager::~ProceduralManager()
{
	sdDelete(pWorldMap);
	sdDelete(pEnemiesMap);
	sdDelete(pItemsMap);
}

void ProceduralManager::BuildWorld(const u32 width, const u32 height, u32 dungeonObjects)
{
	// Initialize rand component
	clRand.Initialize();

	// Initialize common values
	iChanceRoom = 75;
	iChanceCorridor = 25;

	if (dungeonObjects < 1)
		iObjects = 10;
	else
		iObjects = dungeonObjects;

	// We need to ensure a max map size wont be overlaped
	if (width > iXMax)
		iXSize = iXMax;
	else
		iXSize = width;

	if (height > iYMax)
		iYSize = iYMax;
	else
		iYSize = height;

	// Creates the array world map
	pWorldMap = sdNew(u32[iXSize * iYSize]);

	// Creates the array items map
	pItemsMap = sdNew(u32[iXSize * iYSize]);

	// Creates the array enemies map
	pEnemiesMap = sdNew(u32[iXSize * iYSize]);

	// Create the borders and fill the midle with dirt
	for (u32 y = 0; y < iYSize; y++)
	{
		for (u32 x = 0; x < iXSize; x++)
		{
			//Making the borders of unwalkable walls
			if (y == 0)
				SetTile(x, y, tileStoneWall);
			else if (y == iYSize-1)
				SetTile(x, y, tileStoneWall);
			else if (x == 0)
				SetTile(x, y, tileStoneWall);
			else if (x == iXSize-1)
				SetTile(x, y, tileStoneWall);
			//Fill the rest with dirt
			else
				SetTile(x, y, tileBrickFloor);
				//SetTile(x, y, tileUnused);

			// Initialize the enemies
			SetEnemy(x, y, enemyNull);

			// Initialize the items
			SetItem(x, y, itemNull);
		}
	}

	// Create the ramdom rooms
	// Start with making a room in the middle
	MakeRoom(iXSize/2, iYSize/2, 8, 6, clRand.Get(0u, 3u));

	// Keep count of the number of "objects" we've made
	u32 currentFeatures = 1; //+1 for the first room we just made

	// Then we sart the main loop to build the world
	for (int countingTries = 0; countingTries < 1000; countingTries++)
	{
		//check if we've reached our quota
		if (currentFeatures == iObjects)
		{
			break;
		}

		//start with a random wall
		u32 newx = 0;
		u32 xmod = 0;
		u32 newy = 0;
		u32 ymod = 0;
		int validTile = -1;
		//1000 chances to find a suitable object (room or corridor)..
		//(yea, i know it's kinda ugly with a for-loop... -_-')
		for (u32 testing = 0; testing < 1000; testing++)
		{
			newx = clRand.Get(1, iXSize-1);
			newy = clRand.Get(1, iYSize-1);
			validTile = -1;
			if (GetTile(newx, newy) == tileStoneWall)
			{
				//check if we can reach the place
				if (GetTile(newx, newy+1) == tileGrassFloor)
				{
					validTile = 0; //
					xmod = 0;
					ymod = -1;
				}
				else if (GetTile(newx-1, newy) == tileGrassFloor)
				{
					validTile = 1; //
					xmod = +1;
					ymod = 0;
				}
				else if (GetTile(newx, newy-1) == tileGrassFloor)
				{
					validTile = 2; //
					xmod = 0;
					ymod = +1;
				}
				else if (GetTile(newx+1, newy) == tileGrassFloor)
				{
					validTile = 3; //
					xmod = -1;
					ymod = 0;
				}

				//check that we haven't got another door nearby, so we won't get alot of openings besides
				//each other
				if (validTile > -1)
				{
					if (GetTile(newx, newy+1) == tileDoor) //north
						validTile = -1;
					else if (GetTile(newx-1, newy) == tileDoor)//east
						validTile = -1;
					else if (GetTile(newx, newy-1) == tileDoor)//south
						validTile = -1;
					else if (GetTile(newx+1, newy) == tileDoor)//west
						validTile = -1;
				}

				//if we can, jump out of the loop and continue with the rest
				if (validTile > -1) break;
			}
		}
		if (validTile > -1)
		{
			//choose what to build now at our newly found place, and at what direction
			u32 feature = clRand.Get((u32)0, (u32)100);
			if (feature <= iChanceRoom){ //a new room
				if (MakeRoom((newx+xmod), (newy+ymod), 8, 6, validTile))
				{
					currentFeatures++; //add to our quota

					//then we mark the wall opening with a door
					SetTile(newx, newy, tileDoor);

					//clean up infront of the door so we can reach it
					SetTile((newx+xmod), (newy+ymod), tileGrassFloor);
				}
			}
			else if (feature >= iChanceRoom){ //new corridor
				if (MakeCorridor((newx+xmod), (newy+ymod), 6, validTile))
				{
					//same thing here, add to the quota and a door
					currentFeatures++;

					SetTile(newx, newy, tileDoor);
				}
			}
		}
	}

	// Find one place to the UpStairs and DownSairs

	// UpStairs
	bool upStairsPlaced = false;
	int upStairsX = 0;
	int upStairsY = 0;

	while (!upStairsPlaced)
	{
		Log("Trying to find a place for the upStairs");

		upStairsX = clRand.Get(0, iXSize);
		upStairsY = clRand.Get(0, iYSize);

		if (FindFreeRoomPosition(upStairsX, upStairsY))
		{
			SetTile(upStairsX, upStairsY, tileUpStairs);
			upStairsPlaced = true;
		}
	}

	// DownStairs
	bool downStairsPlaced = false;
	int downStairsX = 0;
	int downStairsY = 0;

	while (!downStairsPlaced)
	{
		Log("Trying to find a place for the downStairs");

		downStairsX = clRand.Get(0, iXSize);
		downStairsY = clRand.Get(0, iYSize);

		if (FindFreeRoomPosition(downStairsX, downStairsY))
		{
			SetTile(downStairsX, downStairsY, tileDownStairs);
			downStairsPlaced = true;
		}
	}

	// Sprinkle out the items on the map (enemies, weapons, ammo boxes)
	u32 quantityEnemies = clRand.Get(5u, 10u);
	u32 quantityItems = clRand.Get(6u, 12u);

	u32 xItemPlace = 0;
	u32 yItemPlace = 0;
	u32 xEnemyPlace = 0;
	u32 yEnemyPlace = 0;
	u32 tileToVerify = 0;
	u32 itemToVerify = 0;
	u32 enemyToVerify = 0;

	// Enemies
	while (quantityEnemies > 0)
	{
		xEnemyPlace = clRand.Get(0, iXSize);
		yEnemyPlace = clRand.Get(0, iYSize);
		tileToVerify = GetTile(xEnemyPlace, yEnemyPlace);
		enemyToVerify = GetEnemy(xEnemyPlace, yEnemyPlace);

		// We need to verify if is not a wall or a door and there is empty
		if (	tileToVerify != tileUpStairs &&
				tileToVerify != tileDoor &&
				tileToVerify != tileStoneWall &&
				enemyToVerify == enemyNull)
		{
			SetEnemy(xEnemyPlace, yEnemyPlace, clRand.Get((u32)enemyGrunt, (u32)enemyKnight));
			quantityEnemies--;
		}
	}

	// Items
	while (quantityItems > 0)
	{
		xItemPlace	= clRand.Get(0, iXSize);
		yItemPlace	= clRand.Get(0, iYSize);
		tileToVerify	= GetTile(xItemPlace, yItemPlace);
		enemyToVerify	= GetEnemy(xItemPlace, yItemPlace);
		itemToVerify	= GetItem(xItemPlace, yItemPlace);

		// We need to verify if is not a wall or a door and there is empty
		if (	tileToVerify != tileUpStairs &&
				tileToVerify != tileDownStairs &&
				tileToVerify != tileDoor &&
				tileToVerify != tileStoneWall &&
				tileToVerify != tileBrickFloor &&
				enemyToVerify == enemyNull &&
				itemToVerify == itemNull)
		{
			SetItem(xItemPlace, yItemPlace, clRand.Get((u32)itemHealth, (u32)itemWeaponShockgun));
			quantityItems--;
		}
	}

}

void ProceduralManager::SetTile(u32 x, u32 y, u32 tileType)
{
	pWorldMap[x + iXSize * y] = tileType;
}

int ProceduralManager::GetTile(u32 x, u32 y)
{
	return pWorldMap[x + iXSize * y];
}

void ProceduralManager::SetEnemy(u32 x, u32 y, u32 enemyType)
{
	pEnemiesMap[x + iXSize * y] = enemyType;
}

int ProceduralManager::GetEnemy(u32 x, u32 y)
{
	return pEnemiesMap[x + iXSize * y];
}

void ProceduralManager::SetItem(u32 x, u32 y, u32 itemType)
{
	pItemsMap[x + iXSize * y] = itemType;
}

int ProceduralManager::GetItem(u32 x, u32 y)
{
	return pItemsMap[x + iXSize * y];
}

int ProceduralManager::GetXSize()
{
	return iXSize;
}

int ProceduralManager::GetYSize()
{
	return iYSize;
}

/*
int ProceduralManager::GetRand(int min, int max)
{
	time_t seed;
	seed = time(NULL) + lOldSeed;
	lOldSeed = seed;

	srand(seed);

	int n = max - min + 1;
	int i = rand() % n;

	if(i < 0)
		i = -i;

	return min + i;
}*/

bool ProceduralManager::MakeRoom(u32 x, u32 y, u32 xlength, u32 ylength, u32 direction)
{
	// Define the dimensions of the room, it should be at least 4x4 tiles (2x2 for walking on, the rest is walls)
	u32 xlen = clRand.Get(4, xlength);
	u32 ylen = clRand.Get(4, ylength);

	// The tile type it's going to be filled with
	u32 floor = tileGrassFloor;
	u32 wall = tileStoneWall;

	// Choose the way it's pointing at
	int dir = 0;
	if (direction > 0 && direction < 4) dir = direction;

	switch(dir){
	case 0:
	//north
		//Check if there's enough space left for it
		for (u32 ytemp = y; ytemp > (y-ylen); ytemp--){
			if (ytemp > iYSize) return false;
			for (u32 xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
				if (xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false; //no space left...
			}
		}

		//we're still here, build
		for (u32 ytemp = y; ytemp > (y-ylen); ytemp--){
			for (u32 xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
				//start with the walls
				if (xtemp == (x-xlen/2)) SetTile(xtemp, ytemp, wall);
				else if (xtemp == (x+(xlen-1)/2)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == y) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y-ylen+1)) SetTile(xtemp, ytemp, wall);
				//and then fill with the floor
				else SetTile(xtemp, ytemp, floor);
			}
		}
		break;
	case 1:
	//east
		for (u32 ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			if (ytemp > iYSize) return false;
			for (u32 xtemp = x; xtemp < (x+xlen); xtemp++){
				if (xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (u32 ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			for (u32 xtemp = x; xtemp < (x+xlen); xtemp++){

				if (xtemp == x) SetTile(xtemp, ytemp, wall);
				else if (xtemp == (x+xlen-1)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y-ylen/2)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y+(ylen-1)/2)) SetTile(xtemp, ytemp, wall);

				else SetTile(xtemp, ytemp, floor);
			}
		}
		break;
	case 2:
	//south
		for (u32 ytemp = y; ytemp < (y+ylen); ytemp++){
			if (ytemp > iYSize) return false;
			for (u32 xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
				if (xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (u32 ytemp = y; ytemp < (y+ylen); ytemp++){
			for (u32 xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){

				if (xtemp == (x-xlen/2)) SetTile(xtemp, ytemp, wall);
				else if (xtemp == (x+(xlen-1)/2)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == y) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y+ylen-1)) SetTile(xtemp, ytemp, wall);

				else SetTile(xtemp, ytemp, floor);
			}
		}
		break;
	case 3:
	//west
		for (u32 ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			if (ytemp > iYSize) return false;
			for (u32 xtemp = x; xtemp > (x-xlen); xtemp--){
				if (xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (u32 ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			for (u32 xtemp = x; xtemp > (x-xlen); xtemp--){

				if (xtemp == x) SetTile(xtemp, ytemp, wall);
				else if (xtemp == (x-xlen+1)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y-ylen/2)) SetTile(xtemp, ytemp, wall);
				else if (ytemp == (y+(ylen-1)/2)) SetTile(xtemp, ytemp, wall);

				else SetTile(xtemp, ytemp, floor);
			}
		}
		break;
	}

	//yay, all done
	return true;
}

bool ProceduralManager::MakeCorridor(u32 x, u32 y, u32 lenght, u32 direction)
{
	u32 len = clRand.Get(2, lenght);
	u32 floor = tileCorridor;
	u32 dir = 0;
	if(direction > 0 && direction < 4) dir = direction;

	u32 xtemp = 0;
	u32 ytemp = 0;

	switch(dir)
	{
		case 0:
		{
			if(x > iXSize) return false;
			else xtemp = x;

			for(ytemp = y; ytemp > (y-len); ytemp--)
			{
				if(ytemp > iYSize) return false;
				if(GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}

			for(ytemp = y; ytemp > (y - len); ytemp--)
			{
				SetTile(xtemp, ytemp, floor);
			}
			break;

		}
		case 1:
		{
			if(y > iYSize) return false;
			else ytemp = y;

			for(xtemp = x; xtemp < (x + len); xtemp++)
			{
				if(xtemp > iXSize) return false;
				if(GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}

			for(xtemp = x; xtemp < (x + len); xtemp++)
			{
				SetTile(xtemp, ytemp, floor);
			}
			break;
		}
		case 2:
		{
			if(x > iXSize) return false;
			else xtemp = x;

			for(ytemp = y; ytemp < (y + len); ytemp++)
			{
				if(ytemp > iYSize) return false;
				if(GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
			for (ytemp = y; ytemp < (y+len); ytemp++){
				SetTile(xtemp, ytemp, floor);
			}
		break;
		}
		case 3:
		{
			if (ytemp > iYSize) return false;
			else ytemp = y;

			for (xtemp = x; xtemp > (x-len); xtemp--){
				if (xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}

			for (xtemp = x; xtemp > (x-len); xtemp--){
				SetTile(xtemp, ytemp, floor);
			}
			break;
		}
	}
	//woot, we're still here! let's tell the other guys we're done!!
	return true;
}

bool ProceduralManager::FindFreeRoomPosition(u32 x, u32 y)
{
	if (GetTile(x, y) == tileGrassFloor || GetTile(x, y) == tileBrickFloor)
	{
		return true;
	}
	else
	{
		return false;
	}
}
