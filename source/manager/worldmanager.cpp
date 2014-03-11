#include "worldmanager.h"
#include "../entity/entityfactory.h"
#include "../entity/entity.h"
#include <api/yajl/JsonWriter.h>
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
	int upStairsX = 0;
	int upStairsY = 0;

	JsonWriter jsonWriter;

	// Open node Root
	jsonWriter.OpenNode();

	jsonWriter.WriteString("orientation", "orthogonal");
	jsonWriter.WriteU32("tileheight"	, 40);
	jsonWriter.WriteU32("tilewidth"		, 40);
	jsonWriter.WriteU32("height"		, 30);
	jsonWriter.WriteU32("width"			, 30);
	jsonWriter.WriteU32("version"		, 1);

	// Open node Properties
	jsonWriter.OpenNode("properties");

		jsonWriter.WriteString("NextLevel", "dungeon.scene");

	// Close node Properties
	jsonWriter.CloseNode();

	// Open array Layers
	jsonWriter.OpenArray("layers");

		// Open node Background
		jsonWriter.OpenNode();

			jsonWriter.WriteString("name"	, "Background");
			jsonWriter.WriteString("type"	, "tilelayer");
			jsonWriter.WriteU32	("opacity"	, 1);
			jsonWriter.WriteU32("height"	, 30);
			jsonWriter.WriteU32("width"		, 30);
			jsonWriter.WriteU32("x"			, 0);
			jsonWriter.WriteU32("y"			, 0);
			jsonWriter.WriteBool("visible"	, true);

			// Open array Data
			jsonWriter.OpenArray("data");
			for (int y = 0; y < proceduralManager.GetYSize(); y++)
			{
				for (int x = 0; x < proceduralManager.GetXSize(); x++)
				{
					jsonWriter.WriteU32(proceduralManager.GetTile(x, y));

					if (proceduralManager.GetTile(x, y) == ProceduralManager::tileUpStairs)
					{
						upStairsX = x;
						upStairsY = y;
					}
				}
			}
			// Close array Data
			jsonWriter.CloseArray();

		// Close node Background
		jsonWriter.CloseNode();

		// Open node Colliders
		jsonWriter.OpenNode();

			jsonWriter.WriteString("name"	, "Colliders");
			jsonWriter.WriteString("type"	, "objectgroup");
			jsonWriter.WriteU32("opacity"	, 1);
			jsonWriter.WriteU32("height"	, 30);
			jsonWriter.WriteU32("width"		, 30);
			jsonWriter.WriteU32("x"			, 0);
			jsonWriter.WriteU32("y"			, 0);
			jsonWriter.WriteBool("visible"	, true);

			//Open array Objects
			jsonWriter.OpenArray("objects");

			for (int y = 0; y < proceduralManager.GetYSize(); y++)
			{
				for (int x = 0; x < proceduralManager.GetXSize(); x++)
				{
					if (proceduralManager.GetTile(x, y) == ProceduralManager::tileStoneWall)
					{
						// Open node Collider
						jsonWriter.OpenNode();

							jsonWriter.WriteString("name"	, "");
							jsonWriter.WriteString("type"	, "");
							jsonWriter.WriteU32("height"	, 40);
							jsonWriter.WriteU32("width"		, 40);
							jsonWriter.WriteU32("x"			, (x > 0) ? x * 40 : x);
							jsonWriter.WriteU32("y"			, (y > 0) ? y * 40 : y);
							jsonWriter.WriteBool("visible"	, true);

						// Close node Collider
						jsonWriter.CloseNode();
					}
				}
			}

			//Close array Objects
			jsonWriter.CloseArray();

		// Close node Colliders
		jsonWriter.CloseNode();

		// Open node Game
		jsonWriter.OpenNode();

			jsonWriter.WriteString(	"name",		"Game");
			jsonWriter.WriteString(	"type",		"objectgroup");
			jsonWriter.WriteU32	(	"opacity",	1);
			jsonWriter.WriteU32(	"height",	30);
			jsonWriter.WriteU32(	"width",	30);
			jsonWriter.WriteU32(	"x",		0);
			jsonWriter.WriteU32(	"y",		0);
			jsonWriter.WriteBool(	"visible",	true);

			//Open array Objects
			jsonWriter.OpenArray("objects");

				// Open node OptimistPlayer
				jsonWriter.OpenNode();

					jsonWriter.WriteString("name"	, "OptimistPlayer");
					jsonWriter.WriteString("type"	, "");
					jsonWriter.WriteU32("height"	, 0);
					jsonWriter.WriteU32("width"		, 0);
					jsonWriter.WriteU32("x"			, upStairsX * 40 + 20);
					jsonWriter.WriteU32("y"			, upStairsY * 40 + 20);
					jsonWriter.WriteBool("visible"	, true);

					// Open node Properties
					jsonWriter.OpenNode("properties");

						jsonWriter.WriteString(	"Class", "OptimistPlayer");

					// Close node Properties
					jsonWriter.CloseNode();

				// Close node OptimistPlayer
				jsonWriter.CloseNode();

			//Close array Objects
			jsonWriter.CloseArray();

		// Close node Game
		jsonWriter.CloseNode();

	// Close array Layers
	jsonWriter.CloseArray();

	// Open array Tilesets
	jsonWriter.OpenArray("tilesets");

		// Open node TileSet
		jsonWriter.OpenNode();

			jsonWriter.WriteString("name"		,"ground_tileset");
			jsonWriter.WriteString("image"		, "textures/quakerl_tileset.png");
			jsonWriter.WriteU32("firstgid"		, 1);
			jsonWriter.WriteU32("imageheight"	, 400);
			jsonWriter.WriteU32("imagewidth"	, 40);
			jsonWriter.WriteU32("margin"		, 0);
			jsonWriter.WriteU32("spacing"		, 0);
			jsonWriter.WriteU32("tileheight"	, 40);
			jsonWriter.WriteU32("tilewidth"		, 40);

			// Open node Properties
			jsonWriter.OpenNode("properties");

			// Close node Properties
			jsonWriter.CloseNode();

		// Open node TileSet
		jsonWriter.CloseNode();

	// Close array Tilesets
	jsonWriter.CloseArray();

	// Close node Root
	jsonWriter.CloseNode();

	if (jsonWriter.Save(mapFileName))
		return mapFileName;
	else
		return "Error create map";
}
