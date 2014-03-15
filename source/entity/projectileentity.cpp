#include "projectileentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("Projectile", ProjectileEntity)

#define SLEEP_TIME 10

ProjectileEntity::ProjectileEntity()
	: SpriteEntity("Projectile", "Projectile")
	, fSleepConfig(SLEEP_TIME)
	, fSleep(SLEEP_TIME)
	, fSpeedFactor(1.0f)
{
}

ProjectileEntity::~ProjectileEntity()
{
}

void ProjectileEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	b2Vec2 customSize(50, 40);

	clSensor.Load(*pSprite, true, &customSize, this);

	String sleep = metadata.GetProperty("Sleep");
	if (!sleep.empty())
	{
		sscanf(sleep.c_str(), "%f", &fSleep);
		fSleepConfig = fSleep;
	}

	String speedFactor = metadata.GetProperty("SpeedFactor");
	if (!speedFactor.empty())
	{
		sscanf(speedFactor.c_str(), "%f", &fSpeedFactor);
	}
}

Vector3f ProjectileEntity::GetPosition()
{
	return pSprite->GetPosition();
}

Sprite *ProjectileEntity::GetSprite() const
{
	return pSprite;
}

void ProjectileEntity::Update(f32 dt)
{
	if (fSleep > 0)
	{
		fSleep -= dt;
		if (fSleep < 0)
		{
			this->Activate();
		}
	}
	else
	{
		/*
		if (pTarget == NULL)
		{
			Log("No target to track");
			return;
		}*/

		b2Vec2 dir = b2Vec2(0,0); //pTarget->GetBodyPosition() - clSensor.GetBodyPosition();

		f32 distance = dir.Normalize();
		if (distance > 0.03f)
		{
			//Go faster down to help player jump
			if (dir.y > 0)
			{
				f32 dy = dt * 2;
				dir.y = std::min(dy, dir.y);
				dir.x *= dt;
			}
			else
			{
				dir *= dt;
				dir.y /= 2;
			}

			distance = distance / 1.5f;
			if (distance < 1)
				distance = 1;

			dir *= distance;
			dir *= fSpeedFactor;
			dir += clSensor.GetBodyPosition();

			clSensor.SetBodyPosition(dir);
		}
	}
}

void ProjectileEntity::Activate()
{
	fSleep = 0;
	gSoundManager->Play(SND_WAKEUP);
}

void ProjectileEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Log("DeathEntity colidiu");

		Entity *other = event.GetOtherEntity();
		if (other != NULL && other->GetClassName() == "Player")
		{
			PlayerEntity *player = static_cast<PlayerEntity *>(other);
			if (fSleep <= 0)
			{
				if (player->OnDamage(10))
				{
					gSoundManager->Play(SND_DAMAGE);
					gGameScene->RemoveLife();
				}
			}
		}
	}
}
