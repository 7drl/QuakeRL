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
	// Initialize rand component
	time_t seed;
	seed = time(nullptr);
	clRand.Initialize(u32(seed));
}

ProceduralManager::~ProceduralManager()
{
	sdDelete(pWorldMap);
	sdDelete(pEnemiesMap);
	sdDelete(pItemsMap);
}

void ProceduralManager::BuildWorld(const u32 width, const u32 height, u32 dungeonObjects)
{
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
				this->SetTile(x, y, tileStoneWall);
			else if (y == iYSize - 1)
				this->SetTile(x, y, tileStoneWall);
			else if (x == 0)
				this->SetTile(x, y, tileStoneWall);
			else if (x == iXSize - 1)
				this->SetTile(x, y, tileStoneWall);
			//Fill the rest with dirt
			else
				this->SetTile(x, y, tileBrickFloor);
				//SetTile(x, y, tileUnused);

			// Initialize the enemies
			this->SetEnemy(x, y, enemyNull);

			// Initialize the items
			this->SetItem(x, y, itemNull);
		}
	}

	// Create the ramdom rooms
	// Start with making a room in the middle
	this->MakeRoom(iXSize / 2, iYSize / 2, 8, 6, clRand.Get(0u, 3u));

	// Keep count of the number of "objects" we've made
	u32 currentFeatures = 1; //+1 for the first room we just made

	// Then we sart the main loop to build the world
	for (s32 countingTries = 0; countingTries < 1000; countingTries++)
	{
		//check if we've reached our quota
		if (currentFeatures == iObjects)
			break;

		//start with a random wall
		u32 newx = 0;
		u32 xmod = 0;
		u32 newy = 0;
		u32 ymod = 0;
		s32 validTile = -1;
		//1000 chances to find a suitable object (room or corridor)..
		//(yea, i know it's kinda ugly with a for-loop... -_-')
		for (u32 testing = 0; testing < 1000; testing++)
		{
			newx = clRand.Get(1, iXSize - 1);
			newy = clRand.Get(1, iYSize - 1);
			validTile = -1;
			if (this->GetTile(newx, newy) == tileStoneWall)
			{
				//check if we can reach the place
				if (this->GetTile(newx, newy + 1) == tileBrickRoomFloor)
				{
					validTile = 0; //
					xmod = 0;
					ymod = -1;// BUG HERE.. u32??
				}
				else if (this->GetTile(newx - 1, newy) == tileBrickRoomFloor)
				{
					validTile = 1; //
					xmod = +1;
					ymod = 0;
				}
				else if (this->GetTile(newx, newy - 1) == tileBrickRoomFloor)
				{
					validTile = 2; //
					xmod = 0;
					ymod = +1;
				}
				else if (this->GetTile(newx + 1, newy) == tileBrickRoomFloor)
				{
					validTile = 3; //
					xmod = -1;//BUG HERE.. u32??
					ymod = 0;
				}

				//check that we haven't got another door nearby, so we won't get alot of openings besides
				//each other
				if (validTile > -1)
				{
					if (this->GetTile(newx, newy + 1) == tileDoor) //north
						validTile = -1;
					else if (this->GetTile(newx - 1, newy) == tileDoor)//east
						validTile = -1;
					else if (this->GetTile(newx, newy - 1) == tileDoor)//south
						validTile = -1;
					else if (this->GetTile(newx + 1, newy) == tileDoor)//west
						validTile = -1;
				}

				//if we can, jump out of the loop and continue with the rest
				if (validTile > -1)
					break;
			}
		}

		if (validTile > -1)
		{
			//choose what to build now at our newly found place, and at what direction
			u32 feature = clRand.Get(0u, 100u);
			if (feature <= iChanceRoom) //a new room
			{
				if (this->MakeRoom((newx + xmod), (newy + ymod), 8, 6, validTile))
				{
					currentFeatures++; //add to our quota

					//then we mark the wall opening with a door
					this->SetTile(newx, newy, tileDoor);

					//clean up infront of the door so we can reach it
					this->SetTile((newx + xmod), (newy + ymod), tileBrickRoomFloor);
				}
			}
			else if (feature >= iChanceRoom) //new corridor
			{
				if (this->MakeCorridor((newx + xmod), (newy + ymod), 6, validTile))
				{
					//same thing here, add to the quota and a door
					currentFeatures++;

					this->SetTile(newx, newy, tileDoor);
				}
			}
		}
	}

	// Find one place to the UpStairs and DownSairs

	// UpStairs
	bool upStairsPlaced = false;
	s32 upStairsX = 0;
	s32 upStairsY = 0;

	while (!upStairsPlaced)
	{
		Log("Trying to find a place for the upStairs");

		upStairsX = clRand.Get(0, iXSize);
		upStairsY = clRand.Get(0, iYSize);

		if (this->FindFreeRoomPosition(upStairsX, upStairsY))
		{
			this->SetTile(upStairsX, upStairsY, tileUpStairs);
			upStairsPlaced = true;
		}
	}

	// DownStairs
	bool downStairsPlaced = false;
	s32 downStairsX = 0;
	s32 downStairsY = 0;

	while (!downStairsPlaced)
	{
		Log("Trying to find a place for the downStairs");

		downStairsX = clRand.Get(0, iXSize);
		downStairsY = clRand.Get(0, iYSize);

		if (this->FindFreeRoomPosition(downStairsX, downStairsY))
		{
			this->SetTile(downStairsX, downStairsY, tileDownStairs);
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
		tileToVerify = this->GetTile(xEnemyPlace, yEnemyPlace);
		enemyToVerify = this->GetEnemy(xEnemyPlace, yEnemyPlace);

		// We need to verify if is not a wall or a door and there is empty
		if (	tileToVerify != tileUpStairs &&
				tileToVerify != tileDoor &&
				tileToVerify != tileStoneWall &&
				enemyToVerify == enemyNull)
		{
			this->SetEnemy(xEnemyPlace, yEnemyPlace, clRand.Get(u32(enemyGrunt), u32(enemyKnight)));
			quantityEnemies--;
		}
	}

	// Items
	while (quantityItems > 0)
	{
		xItemPlace	= clRand.Get(0, iXSize);
		yItemPlace	= clRand.Get(0, iYSize);
		tileToVerify	= this->GetTile(xItemPlace, yItemPlace);
		enemyToVerify	= this->GetEnemy(xItemPlace, yItemPlace);
		itemToVerify	= this->GetItem(xItemPlace, yItemPlace);

		// We need to verify if is not a wall or a door and there is empty
		if (	tileToVerify != tileUpStairs &&
				tileToVerify != tileDownStairs &&
				tileToVerify != tileDoor &&
				tileToVerify != tileStoneWall &&
				tileToVerify != tileBrickFloor &&
				enemyToVerify == enemyNull &&
				itemToVerify == itemNull)
		{
			this->SetItem(xItemPlace, yItemPlace, clRand.Get(u32(itemHealth), u32(itemWeaponShockgun)));
			quantityItems--;
		}
	}
}

void ProceduralManager::SetTile(u32 x, u32 y, u32 tileType)
{
	pWorldMap[x + iXSize * y] = tileType;
}

u32 ProceduralManager::GetTile(u32 x, u32 y)
{
	return pWorldMap[x + iXSize * y];
}

void ProceduralManager::SetEnemy(u32 x, u32 y, u32 enemyType)
{
	pEnemiesMap[x + iXSize * y] = enemyType;
}

u32 ProceduralManager::GetEnemy(u32 x, u32 y)
{
	return pEnemiesMap[x + iXSize * y];
}

void ProceduralManager::SetItem(u32 x, u32 y, u32 itemType)
{
	pItemsMap[x + iXSize * y] = itemType;
}

u32 ProceduralManager::GetItem(u32 x, u32 y)
{
	return pItemsMap[x + iXSize * y];
}

u32 ProceduralManager::GetXSize()
{
	return iXSize;
}

u32 ProceduralManager::GetYSize()
{
	return iYSize;
}

bool ProceduralManager::MakeRoom(u32 x, u32 y, u32 xlength, u32 ylength, u32 direction)
{
	// Define the dimensions of the room, it should be at least 4x4 tiles (2x2 for walking on, the rest is walls)
	u32 xlen = clRand.Get(4, xlength);
	u32 ylen = clRand.Get(4, ylength);

	// The tile type it's going to be filled with
	u32 floor = tileBrickRoomFloor;
	u32 wall = tileStoneWall;

	// Choose the way it's pointing at
	s32 dir = 0;
	if (direction > 0 && direction < 4)
		dir = direction;

	switch (dir)
	{
		case 0: //north
		{
			//Check if there's enough space left for it
			for (u32 ytemp = y; ytemp > (y - ylen); ytemp--)
			{
				if (ytemp > iYSize)
					return false;

				for (u32 xtemp = (x - xlen / 2); xtemp < (x + (xlen + 1) / 2); xtemp++)
				{
					if (xtemp > iXSize)
						return false;

					if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
						return false; //no space left...
				}
			}

			//we're still here, build
			for (u32 ytemp = y; ytemp > (y - ylen); ytemp--)
			{
				for (u32 xtemp = (x - xlen / 2); xtemp < (x + (xlen + 1) / 2); xtemp++)
				{
					//start with the walls
					if (xtemp == (x - xlen / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (xtemp == (x + (xlen - 1) / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == y) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y - ylen + 1)) this->SetTile(xtemp, ytemp, wall);
					//and then fill with the floor
					else this->SetTile(xtemp, ytemp, floor);
				}
			}
		}
		break;

		case 1: //east
		{
			for (u32 ytemp = (y - ylen / 2); ytemp < (y + (ylen + 1) / 2); ytemp++)
			{
				if (ytemp > iYSize)
					return false;

				for (u32 xtemp = x; xtemp < (x + xlen); xtemp++)
				{
					if (xtemp > iXSize)
						return false;

					if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
						return false;
				}
			}

			for (u32 ytemp = (y - ylen / 2); ytemp < (y + (ylen + 1) / 2); ytemp++)
			{
				for (u32 xtemp = x; xtemp < (x + xlen); xtemp++)
				{
					if (xtemp == x) this->SetTile(xtemp, ytemp, wall);
					else if (xtemp == (x + xlen - 1)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y - ylen / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y + (ylen - 1) / 2)) this->SetTile(xtemp, ytemp, wall);
					else this->SetTile(xtemp, ytemp, floor);
				}
			}
		}
		break;

		case 2: //south
		{
			for (u32 ytemp = y; ytemp < (y + ylen); ytemp++)
			{
				if (ytemp > iYSize)
					return false;

				for (u32 xtemp = (x - xlen / 2); xtemp < (x + (xlen + 1) / 2); xtemp++)
				{
					if (xtemp > iXSize)
						return false;

					if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
						return false;
				}
			}

			for (u32 ytemp = y; ytemp < (y+ylen); ytemp++)
			{
				for (u32 xtemp = (x - xlen / 2); xtemp < (x + (xlen + 1) / 2); xtemp++)
				{
					if (xtemp == (x - xlen / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (xtemp == (x + (xlen - 1) / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == y) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y + ylen - 1)) this->SetTile(xtemp, ytemp, wall);
					else this->SetTile(xtemp, ytemp, floor);
				}
			}
		}
		break;

		case 3: //west
		{
			for (u32 ytemp = (y - ylen / 2); ytemp < (y + (ylen + 1) / 2); ytemp++)
			{
				if (ytemp > iYSize)
					return false;

				for (u32 xtemp = x; xtemp > (x - xlen); xtemp--)
				{
					if (xtemp > iXSize)
						return false;

					if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
						return false;
				}
			}

			for (u32 ytemp = (y - ylen / 2); ytemp < (y + (ylen + 1) / 2); ytemp++)
			{
				for (u32 xtemp = x; xtemp > (x - xlen); xtemp--)
				{
					if (xtemp == x) this->SetTile(xtemp, ytemp, wall);
					else if (xtemp == (x - xlen + 1)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y - ylen / 2)) this->SetTile(xtemp, ytemp, wall);
					else if (ytemp == (y + (ylen - 1) / 2)) this->SetTile(xtemp, ytemp, wall);
					else this->SetTile(xtemp, ytemp, floor);
				}
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

	if (direction > 0 && direction < 4)
		dir = direction;

	u32 xtemp = 0;
	u32 ytemp = 0;
	switch (dir)
	{
		case 0:
		{
			if (x > iXSize)
				return false;

			xtemp = x;
			for (ytemp = y; ytemp > (y-len); ytemp--)
			{
				if (ytemp > iYSize)
					return false;

				if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
					return false;
			}

			for (ytemp = y; ytemp > (y - len); ytemp--)
				this->SetTile(xtemp, ytemp, floor);
		}
		break;

		case 1:
		{
			if (y > iYSize)
				return false;

			ytemp = y;
			for (xtemp = x; xtemp < (x + len); xtemp++)
			{
				if (xtemp > iXSize)
					return false;

				if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
					return false;
			}

			for (xtemp = x; xtemp < (x + len); xtemp++)
				this->SetTile(xtemp, ytemp, floor);
		}
		break;

		case 2:
		{
			if (x > iXSize)
				return false;

			xtemp = x;
			for (ytemp = y; ytemp < (y + len); ytemp++)
			{
				if (ytemp > iYSize)
					return false;

				if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
					return false;
			}
			for (ytemp = y; ytemp < (y+len); ytemp++)
				this->SetTile(xtemp, ytemp, floor);
		}
		break;

		case 3:
		{
			if (ytemp > iYSize)
				return false;

			ytemp = y;
			for (xtemp = x; xtemp > (x - len); xtemp--)
			{
				if (xtemp > iXSize)
					return false;

				if (this->GetTile(xtemp, ytemp) != tileBrickFloor)
					return false;
			}

			for (xtemp = x; xtemp > (x - len); xtemp--)
				this->SetTile(xtemp, ytemp, floor);
		}
		break;
	}
	//woot, we're still here! let's tell the other guys we're done!!
	return true;
}

bool ProceduralManager::FindFreeRoomPosition(u32 x, u32 y)
{
	return (this->GetTile(x, y) == tileBrickRoomFloor || this->GetTile(x, y) == tileBrickFloor);
}
