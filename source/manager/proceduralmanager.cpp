#include "proceduralmanager.h"
#include "Memory.h"

ProceduralManager::ProceduralManager()
	: pWorldMap(nullptr)
	, iXMax(30)
	, iYMax(30)
	, iXSize(0)
	, iYSize(0)
	, lOldSeed(0)
	, iObjects(0)
	, iChanceRoom(0)
	, iChanceCorridor(0)
{

}

ProceduralManager::~ProceduralManager()
{
	sdDelete(pWorldMap);
}

void ProceduralManager::BuildWorld(const int width, const int height, int dungeonObjects)
{
	// Initialize commom values
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

	// Creates the array map
	pWorldMap = sdNew(int[iXSize * iYSize]);

	// Create the borders and fill the midle with dirt
	for (int y = 0; y < iYSize; y++)
	{
		for (int x = 0; x < iXSize; x++)
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
		}
	}

	// Create the ramdom rooms
	// Start with making a room in the middle
	MakeRoom(iXSize/2, iYSize/2, 8, 6, GetRand(0,3));

	// Keep count of the number of "objects" we've made
	int currentFeatures = 1; //+1 for the first room we just made

	// Then we sart the main loop to build the world
	for (int countingTries = 0; countingTries < 1000; countingTries++)
	{
		//check if we've reached our quota
		if (currentFeatures == iObjects)
		{
			break;
		}

		//start with a random wall
		int newx = 0;
		int xmod = 0;
		int newy = 0;
		int ymod = 0;
		int validTile = -1;
		//1000 chances to find a suitable object (room or corridor)..
		//(yea, i know it's kinda ugly with a for-loop... -_-')
		for (int testing = 0; testing < 1000; testing++){
			newx = GetRand(1, iXSize-1);
			newy = GetRand(1, iYSize-1);
			validTile = -1;
			if (GetTile(newx, newy) == tileStoneWall || GetTile(newx, newy) == tileCorridor){
				//check if we can reach the place
				if (GetTile(newx, newy+1) == tileGrassFloor || GetTile(newx, newy+1) == tileCorridor){
					validTile = 0; //
					xmod = 0;
					ymod = -1;
				}
				else if (GetTile(newx-1, newy) == tileGrassFloor || GetTile(newx-1, newy) == tileCorridor){
					validTile = 1; //
					xmod = +1;
					ymod = 0;
				}
				else if (GetTile(newx, newy-1) == tileGrassFloor || GetTile(newx, newy-1) == tileCorridor){
					validTile = 2; //
					xmod = 0;
					ymod = +1;
				}
				else if (GetTile(newx+1, newy) == tileGrassFloor || GetTile(newx+1, newy) == tileCorridor){
					validTile = 3; //
					xmod = -1;
					ymod = 0;
				}

				//check that we haven't got another door nearby, so we won't get alot of openings besides
				//each other
				if (validTile > -1){
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
		if (validTile > -1){
			//choose what to build now at our newly found place, and at what direction
			int feature = GetRand(0, 100);
			if (feature <= iChanceRoom){ //a new room
				if (MakeRoom((newx+xmod), (newy+ymod), 8, 6, validTile)){
					currentFeatures++; //add to our quota

					//then we mark the wall opening with a door
					SetTile(newx, newy, tileDoor);

					//clean up infront of the door so we can reach it
					SetTile((newx+xmod), (newy+ymod), tileGrassFloor);
				}
			}
			else if (feature >= iChanceRoom){ //new corridor
				if (MakeCorridor((newx+xmod), (newy+ymod), 6, validTile)){
					//same thing here, add to the quota and a door
					currentFeatures++;

					SetTile(newx, newy, tileDoor);
				}
			}
		}
	}
}

void ProceduralManager::SetTile(int x, int y, int tileType)
{
	pWorldMap[x + iXSize * y] = tileType;
}

int ProceduralManager::GetTile(int x, int y)
{
	return pWorldMap[x + iXSize * y];
}

int ProceduralManager::GetXSize()
{
	return iXSize;
}

int ProceduralManager::GetYSize()
{
	return iYSize;
}

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
}

bool ProceduralManager::MakeRoom(int x, int y, int xlength, int ylength, int direction)
{
	// Define the dimensions of the room, it should be at least 4x4 tiles (2x2 for walking on, the rest is walls)
	int xlen = GetRand(4, xlength);
	int ylen = GetRand(4, ylength);

	// The tile type it's going to be filled with
	int floor = tileGrassFloor;
	int wall = tileStoneWall;

	// Choose the way it's pointing at
	int dir = 0;
	if (direction > 0 && direction < 4) dir = direction;

	switch(dir){
	case 0:
	//north
		//Check if there's enough space left for it
		for (int ytemp = y; ytemp > (y-ylen); ytemp--){
			if (ytemp < 0 || ytemp > iYSize) return false;
			for (int xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
				if (xtemp < 0 || xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false; //no space left...
			}
		}

		//we're still here, build
		for (int ytemp = y; ytemp > (y-ylen); ytemp--){
			for (int xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
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
		for (int ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			if (ytemp < 0 || ytemp > iYSize) return false;
			for (int xtemp = x; xtemp < (x+xlen); xtemp++){
				if (xtemp < 0 || xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (int ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			for (int xtemp = x; xtemp < (x+xlen); xtemp++){

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
		for (int ytemp = y; ytemp < (y+ylen); ytemp++){
			if (ytemp < 0 || ytemp > iYSize) return false;
			for (int xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){
				if (xtemp < 0 || xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (int ytemp = y; ytemp < (y+ylen); ytemp++){
			for (int xtemp = (x-xlen/2); xtemp < (x+(xlen+1)/2); xtemp++){

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
		for (int ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			if (ytemp < 0 || ytemp > iYSize) return false;
			for (int xtemp = x; xtemp > (x-xlen); xtemp--){
				if (xtemp < 0 || xtemp > iXSize) return false;
				if (GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
		}

		for (int ytemp = (y-ylen/2); ytemp < (y+(ylen+1)/2); ytemp++){
			for (int xtemp = x; xtemp > (x-xlen); xtemp--){

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

bool ProceduralManager::MakeCorridor(int x, int y, int lenght, int direction)
{
	int len = GetRand(2, lenght);
	int floor = tileCorridor;
	int dir = 0;
	if(direction > 0 && direction < 4) dir = direction;

	int xtemp = 0;
	int ytemp = 0;

	switch(dir)
	{
		case 0:
		{
			if(x < 0 || x > iXSize) return false;
			else xtemp = x;

			for(ytemp = y; ytemp > (y-len); ytemp--)
			{
				if(ytemp < 0 || ytemp > iYSize) return false;
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
			if(y < 0 || y > iYSize) return false;
			else ytemp = y;

			for(xtemp = x; xtemp < (x + len); xtemp++)
			{
				if(xtemp < 0 || xtemp > iXSize) return false;
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
			if(x < 0 || x > iXSize) return false;
			else xtemp = x;

			for(ytemp = y; ytemp < (y + len); ytemp++)
			{
				if(ytemp < 0 || ytemp > iYSize) return false;
				if(GetTile(xtemp, ytemp) != tileBrickFloor) return false;
			}
			for (ytemp = y; ytemp < (y+len); ytemp++){
				SetTile(xtemp, ytemp, floor);
			}
		break;
		}
		case 3:
		{
			if (ytemp < 0 || ytemp > iYSize) return false;
			else ytemp = y;

			for (xtemp = x; xtemp > (x-len); xtemp--){
				if (xtemp < 0 || xtemp > iXSize) return false;
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
