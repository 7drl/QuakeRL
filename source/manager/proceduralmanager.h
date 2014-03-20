#ifndef _PROCEDURAL_MANAGER_H
#define _PROCEDURAL_MANAGER_H

#include "../defines.h"

class ProceduralManager
{
	public:
		ProceduralManager();
		~ProceduralManager();

		void BuildWorld(const u32 width, const u32 height, u32 dungeonObjects);

		void SetTile(u32 x, u32 y, u32 tileType);
		u32 GetTile(u32 x, u32 y);

		void SetItem(u32 x, u32 y, u32 itemType);
		u32 GetItem(u32 x, u32 y);

		void SetEnemy(u32 x, u32 y, u32 enemyType);
		u32 GetEnemy(u32 x, u32 y);

		u32 GetXSize();
		u32 GetYSize();

		enum eTiles
		{
			tileUnused = 0,
			tileBrickFloor,
			tileBrickRoomFloor,
			tileStoneWall,
			tileMetalFloor,
			tileCorridor,
			tileDoor,
			tileUpStairs,
			tileDownStairs,
			tileChest
		};

		enum eEnemies
		{
			enemyNull,
			enemyGrunt ,
			enemyOgre,
			enemyKnight
		};

		enum eItems
		{
			itemNull,
			itemHealth,
			itemLightArmor,
			itemMediumArmor,
			itemHeavyArmor,
			itemShellsAmmo,
			itemNailsAmmo,
			itemRocketsAmmo,
			itemShockAmmo,
			itemWeaponRifle,
			itemWeaponShotgun,
			itemWeaponNailgun,
			itemWeaponHeavyNailgun,
			itemWeaponGrenadeLauncher,
			itemWeaponRocketLauncher,
			itemWeaponShockgun
		};

	private:
		bool MakeRoom(u32 x, u32 y, u32 xlength, u32 ylength, u32 direction);
		bool MakeCorridor(u32 x, u32 y, u32 lenght, u32 direction);
		bool FindFreeRoomPosition(u32 x, u32 y);

	private:
		u32 *pWorldMap;
		u32 *pEnemiesMap;
		u32 *pItemsMap;

		u32 iXMax;
		u32 iYMax;

		u32 iXSize;
		u32 iYSize;

		u32 iObjects;
		u32 iChanceRoom;
		u32 iChanceCorridor;
		Rand clRand;

};

#endif // _PROCEDURAL_MANAGER_H
