#include "worldmanager.h"
#include "../entity/entityfactory.h"
#include "../entity/entity.h"
#include <Writer.h>
#include "proceduralmanager.h"

WorldManager::~WorldManager()
{
	this->Clear();
}

Entity *WorldManager::BuildEntity(MetadataObject &metadata, SceneNode *sprites)
{
	const String &className = metadata.GetProperty("Class");

	Log("%s", className.c_str());

	Entity *ent = EntityFactory::CreateEntity(className);
	if (ent == NULL)
		return NULL;

	ent->Load(metadata, sprites);
	vEntities.push_back(ent);

	return ent;
}

void WorldManager::Clear()
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		sdDelete(*it);
	}

	vEntities.clear();
}

void WorldManager::Update(f32 dt)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		(*it)->Update(dt);
	}
}

Entity *WorldManager::FindEntityByClassName(const String &className)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetClassName().compare(className) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

Entity *WorldManager::FindEntityByName(const String &name)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetName().compare(name) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

void WorldManager::ActivateAllEntites(const String &name)
{
	for (EntitiesVector::iterator it = vEntities.begin(), end = vEntities.end(); it != end; ++it)
	{
		if ((*it)->GetName().compare(name) == 0)
		{
			(*it)->Activate();
		}
	}
}

String WorldManager::GenerateProceduralMap()
{
	// Build the map
	ProceduralManager proceduralManager{};
	proceduralManager.BuildWorld(30, 30, 30);

	// Generate the json file
	String mapFileName(pFileSystem->GetWorkDirectory());
	mapFileName.append("proceduralmap.json");

	// Upstairs posistion
	u32 upStairsX = 0;
	u32 upStairsY = 0;

	// Downstairs position
	u32 downStairsX = 0;
	u32 downStairsY = 0;

	Writer writer;

	// Open node Root
	writer.OpenNode();

	writer.WriteString("orientation", "orthogonal");
	writer.WriteU32("tileheight"	, 40);
	writer.WriteU32("tilewidth"		, 40);
	writer.WriteU32("height"		, 30);
	writer.WriteU32("width"			, 30);
	writer.WriteU32("version"		, 1);

	// Open node Properties
	writer.OpenNode("properties");

		writer.WriteString("NextLevel", "pathfind.scene");

	// Close node Properties
	writer.CloseNode();

	// Open array Layers
	writer.OpenArray("layers");

		// Open node Background
		writer.OpenNode();

			writer.WriteString("name"	, "Background");
			writer.WriteString("type"	, "tilelayer");
			writer.WriteU32	("opacity"	, 1);
			writer.WriteU32("height"	, 30);
			writer.WriteU32("width"		, 30);
			writer.WriteU32("x"			, 0);
			writer.WriteU32("y"			, 0);
			writer.WriteBool("visible"	, true);

			// Open array Data
			writer.OpenArray("data");
			for (u32 y = 0; y < proceduralManager.GetYSize(); y++)
			{
				for (u32 x = 0; x < proceduralManager.GetXSize(); x++)
				{
					writer.WriteU32(proceduralManager.GetTile(x, y));

					if (proceduralManager.GetTile(x, y) == ProceduralManager::eTiles::tileUpStairs)
					{
						upStairsX = x;
						upStairsY = y;
					}

					if (proceduralManager.GetTile(x, y) == ProceduralManager::eTiles::tileDownStairs)
					{
						downStairsX = x;
						downStairsY = y;
					}
				}
			}
			// Close array Data
			writer.CloseArray();

		// Close node Background
		writer.CloseNode();

		// Open node Colliders
		writer.OpenNode();

			writer.WriteString("name"	, "Colliders");
			writer.WriteString("type"	, "objectgroup");
			writer.WriteU32("opacity"	, 1);
			writer.WriteU32("height"	, 30);
			writer.WriteU32("width"		, 30);
			writer.WriteU32("x"			, 0);
			writer.WriteU32("y"			, 0);
			writer.WriteBool("visible"	, true);

			//Open array Objects
			writer.OpenArray("objects");

			for (u32 y = 0; y < proceduralManager.GetYSize(); y++)
			{
				for (u32 x = 0; x < proceduralManager.GetXSize(); x++)
				{
					if (proceduralManager.GetTile(x, y) == ProceduralManager::eTiles::tileStoneWall)
					{
						// Open node Collider
						writer.OpenNode();

							writer.WriteString("name"	, "");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 40);
							writer.WriteU32("width"		, 40);
							writer.WriteU32("x"			, (x > 0) ? x * 40 : x);
							writer.WriteU32("y"			, (y > 0) ? y * 40 : y);
							writer.WriteBool("visible"	, true);

						// Close node Collider
						writer.CloseNode();
					}
				}
			}

			//Close array Objects
			writer.CloseArray();

		// Close node Colliders
		writer.CloseNode();

		// Open node Game
		writer.OpenNode();

			writer.WriteString(	"name",		"Game");
			writer.WriteString(	"type",		"objectgroup");
			writer.WriteU32	(	"opacity",	1);
			writer.WriteU32(	"height",	30);
			writer.WriteU32(	"width",	30);
			writer.WriteU32(	"x",		0);
			writer.WriteU32(	"y",		0);
			writer.WriteBool(	"visible",	true);

			//Open array Objects
			writer.OpenArray("objects");

				// Open node OptimistPlayer
				writer.OpenNode();

					writer.WriteString("name"	, "OptimistPlayer");
					writer.WriteString("type"	, "");
					writer.WriteU32("height"	, 40);
					writer.WriteU32("width"		, 40);
					writer.WriteU32("x"			, upStairsX * 40 + 20);
					writer.WriteU32("y"			, upStairsY * 40 + 20);
					writer.WriteBool("visible"	, true);

					// Open node Properties
					writer.OpenNode("properties");

						writer.WriteString(	"Class", "OptimistPlayer");

					// Close node Properties
					writer.CloseNode();

				// Close node OptimistPlayer
				writer.CloseNode();

				// Open node DownStairs to the Next level
				writer.OpenNode();

					writer.WriteString("name"	, "NextLevel");
					writer.WriteString("type"	, "");
					writer.WriteU32("height"	, 40);
					writer.WriteU32("width"		, 40);
					writer.WriteU32("x"			, downStairsX * 40);
					writer.WriteU32("y"			, downStairsY * 40);
					writer.WriteBool("visible"	, true);

					// Open node Properties
					writer.OpenNode("properties");

						writer.WriteString(	"Class", "Trigger");
						writer.WriteBool("Once", true);

					// Close node Properties
					writer.CloseNode();

				// Close node DownStairs to the Next level
				writer.CloseNode();

				// Here we will open a lot of nodes to create enemies and objects
				for (u32 y = 0; y < proceduralManager.GetYSize(); y++)
				{
					for (u32 x = 0; x < proceduralManager.GetXSize(); x++)
					{
						// Enemies
						if (proceduralManager.GetEnemy(x, y) == ProceduralManager::eEnemies::enemyGrunt)
						{
							// Open node Enemy
							writer.OpenNode();

								writer.WriteString("name"	, "EnemyGrunt");
								writer.WriteString("type"	, "");
								writer.WriteU32("height"	, 0);
								writer.WriteU32("width"		, 0);
								writer.WriteU32("x"			, x * 40 + 20);
								writer.WriteU32("y"			, y * 40 + 20);
								writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Enemy");
									writer.WriteString(	"AttackPower", "6");
									writer.WriteString(	"DefensePower", "3");
									writer.WriteString(	"EnemyId", "1");
									writer.WriteString(	"Life", "4");
									writer.WriteString(	"LifeTotal", "4");

								// Close node Properties
								writer.CloseNode();

							// Close node Enemy
							writer.CloseNode();
						}
						else if (proceduralManager.GetEnemy(x, y) == ProceduralManager::eEnemies::enemyOgre)
						{
							// Open node Enemy
							writer.OpenNode();

								writer.WriteString("name"	, "EnemyOgre");
								writer.WriteString("type"	, "");
								writer.WriteU32("height"	, 0);
								writer.WriteU32("width"		, 0);
								writer.WriteU32("x"			, x * 40 + 20);
								writer.WriteU32("y"			, y * 40 + 20);
								writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Enemy");
									writer.WriteString(	"AttackPower", "10");
									writer.WriteString(	"DefensePower", "3");
									writer.WriteString(	"EnemyId", "2");
									writer.WriteString(	"Level", "1");
									writer.WriteString(	"Life", "8");
									writer.WriteString(	"LifeTotal", "8");

								// Close node Properties
								writer.CloseNode();

							// Close node Enemy
							writer.CloseNode();
						}
						else if (proceduralManager.GetEnemy(x, y) == ProceduralManager::eEnemies::enemyKnight)
						{
							// Open node Enemy
							writer.OpenNode();

								writer.WriteString("name"	, "EnemyKnight");
								writer.WriteString("type"	, "");
								writer.WriteU32("height"	, 0);
								writer.WriteU32("width"		, 0);
								writer.WriteU32("x"			, x * 40 + 20);
								writer.WriteU32("y"			, y * 40 + 20);
								writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Enemy");
									writer.WriteString(	"AttackPower", "8");
									writer.WriteString(	"DefensePower", "3");
									writer.WriteString(	"EnemyId", "3");
									writer.WriteString(	"Level", "1");
									writer.WriteString(	"Life", "6");
									writer.WriteString(	"LifeTotal", "6");

								// Close node Properties
								writer.CloseNode();

							// Close node Enemy
							writer.CloseNode();
						}

						// Items
						if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemHealth)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "HealthPotion");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "HealthPotion");
									writer.WriteString( "id", "10");
									writer.WriteString(	"Amount", "25");

								// Close node Properties
								writer.CloseNode();

							// Close node Object
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemLightArmor)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Armor");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Armor");
									writer.WriteString( "id", "11");
									writer.WriteString(	"AnimationName", "LightArmor");
									writer.WriteString(	"Amount", "100");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemMediumArmor)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Armor");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Armor");
									writer.WriteString( "id", "12");
									writer.WriteString(	"AnimationName", "MediumArmor");
									writer.WriteString(	"Amount", "150");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemHeavyArmor)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Armor");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Armor");
									writer.WriteString( "id", "13");
									writer.WriteString(	"AnimationName", "HeavyArmor");
									writer.WriteString(	"Amount", "200");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemShellsAmmo)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Ammo");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Ammo");
									writer.WriteString( "id", "14");
									writer.WriteString(	"AnimationName", "ShellsAmmo");
									writer.WriteString(	"Amount", "20");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemNailsAmmo)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Ammo");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Ammo");
									writer.WriteString( "id", "15");
									writer.WriteString(	"AnimationName", "NailsAmmo");
									writer.WriteString(	"Amount", "20");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemRocketsAmmo)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Ammo");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Ammo");
									writer.WriteString( "id", "16");
									writer.WriteString(	"AnimationName", "RocketsAmmo");
									writer.WriteString(	"Amount", "20");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemShockAmmo)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Ammo");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Ammo");
									writer.WriteString( "id", "17");
									writer.WriteString(	"AnimationName", "ShockAmmo");
									writer.WriteString(	"Amount", "20");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						// Weapons
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponRifle)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "2");
									writer.WriteString(	"AnimationName", "Rifle");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponShotgun)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "3");
									writer.WriteString(	"AnimationName", "Shotgun");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponNailgun)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "4");
									writer.WriteString(	"AnimationName", "Nailgun");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponHeavyNailgun)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "5");
									writer.WriteString(	"AnimationName", "HeavyNailgun");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponGrenadeLauncher)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "6");
									writer.WriteString(	"AnimationName", "GrenadeLauncher");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponRocketLauncher)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "7");
									writer.WriteString(	"AnimationName", "RocketLauncher");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
						else if (proceduralManager.GetItem(x, y) == ProceduralManager::eItems::itemWeaponShockgun)
						{
							// Open node Item
							writer.OpenNode();

							writer.WriteString("name"	, "Weapon");
							writer.WriteString("type"	, "");
							writer.WriteU32("height"	, 20);
							writer.WriteU32("width"		, 20);
							writer.WriteU32("x"			, x * 40 + 20);
							writer.WriteU32("y"			, y * 40 + 20);
							writer.WriteBool("visible"	, true);

								// Open node Properties
								writer.OpenNode("properties");

									writer.WriteString(	"Class", "Weapon");
									writer.WriteString( "id", "8");
									writer.WriteString(	"AnimationName", "Shockgun");
									writer.WriteString(	"Amount", "1");

								// Close node Properties
								writer.CloseNode();

							// Close node Item
							writer.CloseNode();
						}
					}
				}
				// Enemies and objects created

			//Close array Objects
			writer.CloseArray();

		// Close node Game
		writer.CloseNode();

	// Close array Layers
	writer.CloseArray();

	// Open array Tilesets
	writer.OpenArray("tilesets");

		// Open node TileSet
		writer.OpenNode();

			writer.WriteString("name"		,"ground_tileset");
			writer.WriteString("image"		, "textures/quakerl_tileset.png");
			writer.WriteU32("firstgid"		, 1);
			writer.WriteU32("imageheight"	, 400);
			writer.WriteU32("imagewidth"	, 40);
			writer.WriteU32("margin"		, 0);
			writer.WriteU32("spacing"		, 0);
			writer.WriteU32("tileheight"	, 40);
			writer.WriteU32("tilewidth"		, 40);

			// Open node Properties
			writer.OpenNode("properties");

			// Close node Properties
			writer.CloseNode();

		// Open node TileSet
		writer.CloseNode();

	// Close array Tilesets
	writer.CloseArray();

	// Close node Root
	writer.CloseNode();

	if (writer.Save(mapFileName))
		return mapFileName;
	else
		return "Error create map";
}
