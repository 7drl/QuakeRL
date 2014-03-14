#ifndef _PROCEDURAL_MANAGER_H
#define _PROCEDURAL_MANAGER_H

#include "../defines.h"

class ProceduralManager
{
	public:
		ProceduralManager();
		virtual ~ProceduralManager();
		virtual void BuildWorld(const u32 width, const u32 height, u32 dungeonObjects);
		virtual void SetTile(u32 x, u32 y, u32 tileType);
		virtual u32 GetTile(u32 x, u32 y);
		virtual void SetItem(u32 x, u32 y, u32 itemType);
		virtual u32 GetItem(u32 x, u32 y);
		virtual void SetEnemy(u32 x, u32 y, u32 enemyType);
		virtual u32 GetEnemy(u32 x, u32 y);
		virtual u32 GetXSize();
		virtual u32 GetYSize();

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

		//virtual int GetRand(int min, int max);
		virtual bool MakeRoom(u32 x, u32 y, u32 xlength, u32 ylength, u32 direction);
		virtual bool MakeCorridor(u32 x, u32 y, u32 lenght, u32 direction);
		virtual bool FindFreeRoomPosition(u32 x, u32 y);

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
