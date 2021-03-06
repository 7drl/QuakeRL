#include "enemyentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../manager/guimanager.h"

#define PIX2M		0.01f
#define M2PIX		(1.0f / PIX2M)

ENTITY_CREATOR("Enemy", EnemyEntity)

EnemyEntity::EnemyEntity()
	: SpriteEntity("Enemy", "Enemy")
	, pTarget(nullptr)
	, pBody(nullptr)
	, fInvicibleTime(0.0f)
	, bPlayerLock(false)
	, bIsDead(false)
	, bIsPlayerFound(false)
	, cPath()
{
	sEnemy.displayName = "Enemy";
}

EnemyEntity::~EnemyEntity()
{
	gPhysics->DestroyBody(pBody);
	pBody = nullptr;
}

void EnemyEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	pSprite->SetZ(-10);
	clSensor.Load(metadata, this);

	if (!metadata.GetProperty("DisplayName").empty())
		sEnemy.displayName = metadata.GetProperty("DisplayName");
	else
		sEnemy.displayName = "Enemy";

	if (!metadata.GetProperty("EnemyId").empty())
		sEnemy.iEnemyId = std::stoi(metadata.GetProperty("EnemyId"));
	else
		sEnemy.iEnemyId = 0;

	if (!metadata.GetProperty("Level").empty())
		sEnemy.iLevel = std::stoi(metadata.GetProperty("Level"));
	else
		sEnemy.iLevel = 1;

	if (!metadata.GetProperty("AttackPower").empty())
		sEnemy.iAttackPower = std::stoi(metadata.GetProperty("AttackPower"));
	else
		sEnemy.iAttackPower = 3;

	if (!metadata.GetProperty("DefensePower").empty())
		sEnemy.iDefensePower = std::stoi(metadata.GetProperty("DefensePower"));
	else
		sEnemy.iDefensePower = 2;

	if (!metadata.GetProperty("Life").empty())
		sEnemy.iLife = std::stoi(metadata.GetProperty("Life"));
	else
		sEnemy.iLife = 5;

	if (!metadata.GetProperty("LifeTotal").empty())
		sEnemy.iLifeTotal = std::stoi(metadata.GetProperty("LifeTotal"));
	else
		sEnemy.iLifeTotal = 5;

	b2Vec2 customSize(40, 40);

	// Change enemy sprites to idle
	LoadEnemyIdleAnimation();

	pBody = gPhysics->CreateKinematicBody(pSprite, &customSize);
	pBody->SetFixedRotation(true);
	pBody->GetFixtureList()->SetUserData(this);
}

void EnemyEntity::Update(f32 dt)
{
	if (fInvicibleTime > 0)
	{
		fInvicibleTime -= dt;
		if (fInvicibleTime <= 0)
		{
			// Change enemy sprites to idle
			LoadEnemyIdleAnimation();

			// Do Damage to the player
			pTarget->OnDamage(sEnemy.iAttackPower);

			// Reset invicible time
			fInvicibleTime = 0;

			if (bIsDead)
				this->pSprite->SetVisible(false);
		}
	}

	if (!pBody)
		return;

	// Search a nerby player
	if (pTarget == nullptr)
		pTarget = static_cast<OptimistPlayerEntity *>(gWorldManager->FindEntityByClassName("OptimistPlayer"));

	if (pTarget != nullptr)
	{
		auto dir = pTarget->GetBodyPosition() - pBody->GetPosition();
		auto distance = dir.Normalize();

		if (distance <= 1.5f && !bPlayerLock)
		{
			// Verify if the player has a target
			if (pTarget->GetEnemyTarget() == nullptr)
			{
				// Register himself to the player as target
				pTarget->SetEnemyTarget(this);

				// Play the awake sound
				PlayEnemyAwakeSound();

				bPlayerLock = true;
				this->SetDisplayName(this->GetDisplayName());
				this->SetLevel(this->GetLevel());
				this->SetLife(this->GetLife());
			}
		}
		else if (bPlayerLock && distance >= 1.0f)
		{
			bPlayerLock = false;
		}
	}
}

void EnemyEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter && !bIsDead)
	{
		Log("ENEMY colidiu");

		auto other = event.GetOtherEntity();
		if (other != nullptr)
		{
			Log("Colisao com: %s", other->GetClassName().c_str());
			if(other->GetClassName() == "OptimistPlayer")
			{
				auto player = static_cast<PlayerEntity *>(other);

				// Stop player movement
				player->StopPlayerMovement();

				auto damageToPlayer = s32((player->GetDefensePower() - sEnemy.iAttackPower) + (rand() % 3 + 1));
				if (damageToPlayer < 0)
					damageToPlayer = 0;

				//Do damage to the player
				player->OnDamage(u32(damageToPlayer));

				auto damageEnemyBase = s32(player->GetAttackPower() - sEnemy.iDefensePower + (rand() % 3 + 1));
				if (damageEnemyBase < 0)
					damageEnemyBase = 0;

				//Receive damage
				this->ReceiveDamage(u32(damageEnemyBase), ItemTypes::Weapons::Axe);
			}
		}
	}
}

bool EnemyEntity::ReceiveDamage(u32 amount, ItemTypes::Weapons weapon)
{
	// Change animation
	if (weapon == ItemTypes::Weapons::Rifle || weapon == ItemTypes::Weapons::Shotgun ||
		weapon == ItemTypes::Weapons::Nailgun || weapon == ItemTypes::Weapons::HeavyNailgun)
	{
		LoadEnemyBloodAnimation();
	}
	else if (weapon == ItemTypes::Weapons::RocketLauncher || weapon == ItemTypes::Weapons::GrenadeLauncher)
	{
		LoadEnemyExplosionAnimation();
	}
	else if (weapon == ItemTypes::Weapons::Shockgun)
	{
		LoadEnemyShockAnimation();
	}
	else
	{
		LoadEnemyBloodAnimation();
	}

	if (fInvicibleTime > 0)
		return false;

	// Receive the damage
	this->SetLife(this->GetLife() - amount);

	// Set animation time
	fInvicibleTime = 0.6f;

	if ((int)this->GetLife() <= 0)
	{
		// Add body to a list to remove
		gPhysics->AddBodyToRemove(pBody);
		pBody = nullptr;
		bIsDead = true;

		// Remove from the players target
		pTarget->SetEnemyTarget(nullptr);
	}

	return true;
}

String EnemyEntity::GetDisplayName() const
{
	return sEnemy.displayName;
}

void EnemyEntity::SetDisplayName(const String &/*displayName*/)
{
	//gGui->SetEnemyName(displayName);
}

u32 EnemyEntity::GetLevel() const
{
	return sEnemy.iLevel;
}

void EnemyEntity::SetLevel(u32 level)
{
	sEnemy.iLevel = level;
	//gGui->SetEnemyLevel(level);
}

u32 EnemyEntity::GetLife() const
{
	return sEnemy.iLife;
}

void EnemyEntity::SetLife(u32 life)
{
	sEnemy.iLife = life;
	//gGui->SetEnemyLife(life, this->sEnemy.iLifeTotal);
}

void EnemyEntity::FindPathToPlayer()
{
	// Find path to the player
	gPathfinderManager->Findpath(pSprite->GetPosition(), pTarget->GetSprite()->GetPosition(), cPath);
	bIsPlayerFound = false;

	if (cPath.GetDirectionSteps().empty())
		return;

	auto dir = cPath.GetDirectionSteps().top();
	if (dir.x != 0.0f)
	{
		if (dir.x > 0.0)
			pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x + (PIX2M * 40), pBody->GetTransform().p.y), 0);
		else
			pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x - (PIX2M * 40), pBody->GetTransform().p.y), 0);
	}

	if (dir.y != 0.0f)
	{
		if (dir.y > 0.0f)
			pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y + (PIX2M * 40)), 0);
		else
			pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y - (PIX2M * 40)), 0);
	}

	while (!cPath.GetDirectionSteps().empty())
		cPath.GetDirectionSteps().pop();
}

b2Vec2 EnemyEntity::GetBodyPosition() const
{
	return pBody->GetPosition();
}

void EnemyEntity::LoadEnemyIdleAnimation()
{
	if (sEnemy.iEnemyId == 1)
		pSprite->SetAnimation("EnemyGrunt");
	else if (sEnemy.iEnemyId == 2)
		pSprite->SetAnimation("EnemyOgre");
	else if (sEnemy.iEnemyId == 3)
		pSprite->SetAnimation("EnemyKnight");
	else
		pSprite->SetAnimation("EnemyGrunt");
}

void EnemyEntity::LoadEnemyBloodAnimation()
{
	if (sEnemy.iEnemyId == 1)
		pSprite->SetAnimation("EnemyGruntBlood");
	else if (sEnemy.iEnemyId == 2)
		pSprite->SetAnimation("EnemyOgreBlood");
	else if (sEnemy.iEnemyId == 3)
		pSprite->SetAnimation("EnemyKnightBlood");
	else
		pSprite->SetAnimation("EnemyGruntBlood");
}

void EnemyEntity::LoadEnemyExplosionAnimation()
{
	if (sEnemy.iEnemyId == 1)
		pSprite->SetAnimation("EnemyGruntExplosion");
	else if (sEnemy.iEnemyId == 2)
		pSprite->SetAnimation("EnemyOgreExplosion");
	else if (sEnemy.iEnemyId == 3)
		pSprite->SetAnimation("EnemyKnightExplosion");
	else
		pSprite->SetAnimation("EnemyGruntExplosion");

}

void EnemyEntity::LoadEnemyShockAnimation()
{
	if (sEnemy.iEnemyId == 1)
		pSprite->SetAnimation("EnemyGruntShock");
	else if (sEnemy.iEnemyId == 2)
		pSprite->SetAnimation("EnemyOgreShock");
	else if (sEnemy.iEnemyId == 3)
		pSprite->SetAnimation("EnemyKnightShock");
	else
		pSprite->SetAnimation("EnemyGruntShock");
}

void EnemyEntity::PlayEnemyAwakeSound()
{
	if (sEnemy.iEnemyId == 1)
		gSoundManager->Play(SND_GRUNT_WAKE);
	else if (sEnemy.iEnemyId == 2)
		gSoundManager->Play(SND_OGRE_WAKE);
	else if (sEnemy.iEnemyId == 3)
		gSoundManager->Play(SND_KNIGHT_WAKE);
	else
		gSoundManager->Play(SND_GRUNT_WAKE);
}

bool EnemyEntity::IsDead() const
{
	return bIsDead;
}
