#include "spriteentity.h"
#include "../scene/gamescene.h"
#include "entityfactory.h"

ENTITY_CREATOR("Sprite", SpriteEntity)

SpriteEntity::SpriteEntity():
	Entity("Sprite"),
	pSprite(NULL),
	pszSpriteName(NULL)
{
}

SpriteEntity::SpriteEntity(const char *className, const char *spriteName)
	: Entity(className)
	, pSprite(NULL)
	, pszSpriteName(spriteName)
{
	SEED_ASSERT(spriteName);
}

SpriteEntity::~SpriteEntity()
{
	gScene->Remove(pSprite);
	sdDelete(pSprite);
}

void SpriteEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	Entity::Load(metadata, sprites);

	const char *spriteObject = pszSpriteName;
	String temp;
	if(spriteObject == NULL)
	{
		temp = metadata.GetProperty("Sprite");
		if(temp.empty())
		{
			Log("No sprite for SpriteEntity");
			return;
		}

		spriteObject = temp.c_str();
	}

	pSprite = sdNew(Sprite(*static_cast<Sprite *>(sprites->GetChildByName(spriteObject))));
	pSprite->SetPosition(metadata.GetPosition());

	gScene->Add(pSprite);
}

void SpriteEntity::Load(MetadataObject &metadata, SceneNode *sprites, const char *spriteName)
{
	Entity::Load(metadata, sprites);

	pSprite = sdNew(Sprite(*static_cast<Sprite *>(sprites->GetChildByName(spriteName))));
	pSprite->SetPosition(metadata.GetPosition());

	gScene->Add(pSprite);
}
