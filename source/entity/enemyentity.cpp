#include "enemyentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../manager/guimanager.h"
#include <cmath>

ENTITY_CREATOR("Enemy", EnemyEntity)

EnemyEntity::EnemyEntity()
	: SpriteEntity("Enemy", "Enemy")
	, pBody(nullptr)
	, fInvicibleTime(0.0f)
	, pTarget(nullptr)
	, bPlayerLock(false)
	, bIsDead(false)
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

	pBody = gPhysics->CreateKinematicBody(pSprite, &customSize);
	pBody->SetFixedRotation(true);
	pBody->GetFixtureList()->SetUserData(this);
}

void EnemyEntity::Update(f32 dt)
{
	if (!pBody)
		return;

	b2Vec2 vel = pBody->GetLinearVelocity();
	pBody->SetLinearVelocity(vel);

	if (fInvicibleTime > 0)
	{
		pSprite->SetVisible(!pSprite->IsVisible());

		fInvicibleTime -= dt;
		if (fInvicibleTime <= 0)
		{
			pSprite->SetVisible(true);
			fInvicibleTime = 0;
		}
	}

	// Search a nerby player
	if (pTarget == nullptr || (pTarget != nullptr && !pTarget->GetIsActive()))
		pTarget = static_cast<OptimistPlayerEntity *>(gWorldManager->FindEntityByClassName("OptimistPlayer"));

	if (pTarget == nullptr || (pTarget != nullptr &&!pTarget->GetIsActive()))
		pTarget = static_cast<RealistPlayerEntity *>(gWorldManager->FindEntityByClassName("RealistPlayer"));

	if (pTarget == nullptr || (pTarget != nullptr && !pTarget->GetIsActive()))
		pTarget = static_cast<PessimistPlayerEntity *>(gWorldManager->FindEntityByClassName("PessimistPlayer"));

	if (pTarget != nullptr && pTarget->GetIsActive())
	{
		// Change enemy sprites
		if (pTarget->GetClassName() == "OptimistPlayer")
		{
			if (sEnemy.iEnemyId == 0)
				pSprite->SetAnimation("OptimistEnemy");
			else if (sEnemy.iEnemyId == 1)
				pSprite->SetAnimation("OptimistEnemy1");
			else if (sEnemy.iEnemyId == 2)
				pSprite->SetAnimation("OptimistEnemy2");
			else
				pSprite->SetAnimation("OptimistEnemy3");
		}
		else if (pTarget->GetClassName() == "RealistPlayer")
		{
			if (sEnemy.iEnemyId == 0)
				pSprite->SetAnimation("RealistEnemy");
			else if (sEnemy.iEnemyId == 1)
				pSprite->SetAnimation("RealistEnemy1");
			else if (sEnemy.iEnemyId == 2)
				pSprite->SetAnimation("RealistEnemy2");
			else
				pSprite->SetAnimation("RealistEnemy3");
		}
		else
		{
			if (sEnemy.iEnemyId == 0)
				pSprite->SetAnimation("PessimistEnemy");
			else if (sEnemy.iEnemyId == 1)
				pSprite->SetAnimation("PessimistEnemy1");
			else if (sEnemy.iEnemyId == 2)
				pSprite->SetAnimation("PessimistEnemy2");
			else
				pSprite->SetAnimation("PessimistEnemy3");
		}

		b2Vec2 dir = pTarget->GetBodyPosition() - pBody->GetPosition();

		f32 distance = dir.Normalize();

		if (distance <= 1.0f && !bPlayerLock)
		{
			pTarget->Talk();
			bPlayerLock = true;
			this->SetDisplayName(this->GetDisplayName());
			this->SetLevel(this->GetLevel());
			this->SetLife(this->GetLife());
			gGui->SelectEnemy(pTarget->GetDisplayName(), this->sEnemy.iEnemyId);
		}
		else if (bPlayerLock && distance >= 1.0f)
		{
			pTarget->Mute();
			bPlayerLock = false;
			gGui->SelectEnemy();
		}
	}
}

void EnemyEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter && !bIsDead)
	{
		Log("ENEMY colidiu");

		Entity *other = event.GetOtherEntity();
		if ((other != nullptr && other->GetClassName() == "OptimistPlayer") ||
			(other != nullptr && other->GetClassName() == "RealistPlayer") ||
			(other != nullptr && other->GetClassName() == "PessimistPlayer"))
		{
			PlayerEntity *player = static_cast<PlayerEntity *>(other);

			// Stop player movement
			player->StopPlayerMovement();
			player->SetIsInputEnabled(false);

			// Define a vector to push the player
			b2Vec2 vecToPush = b2Vec2(0, 0);

			// Find where the player comes
			if (gPhysics->RayCast(pBody, b2Vec2(0, -0.32f)) ||
				gPhysics->RayCast(pBody, b2Vec2(0.16f, -0.32f)) ||
				gPhysics->RayCast(pBody, b2Vec2(-0.16f, -0.32f)))
			{
				Log("Push player up");
				vecToPush = b2Vec2(0.0f, -1.0f);
			}
			else if (gPhysics->RayCast(pBody, b2Vec2(0, 0.32f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(0.16f, 0.32f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(-0.16f, 0.32f)))
			{
				Log("Push player down");
				vecToPush = b2Vec2(0.0f, 1.0f);
			}
			else if (gPhysics->RayCast(pBody, b2Vec2(-0.32f, 0.0f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(-0.32f, 0.16f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(-0.32f, -0.16f)))
			{
				Log("Push player left");
				vecToPush = b2Vec2(-1.0f, 0.0f);
			}
			else if (gPhysics->RayCast(pBody, b2Vec2(0.32f, 0.0f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(0.32f, 0.16f)) ||
					 gPhysics->RayCast(pBody, b2Vec2(0.32f, -0.16f)))
			{
				Log("Push player right");
				vecToPush = b2Vec2(1.0f, 0.0f);
			}

			s32 damageToPlayer = (player->GetDefensePower() - sEnemy.iAttackPower) + (rand() % 3 + 1);
			if (damageToPlayer < 0)
				damageToPlayer = 0;

			//Do damage to the player
			player->OnDamage(vecToPush, u32(damageToPlayer));

			s32 damageEnemyBase = player->GetAttackPower() - sEnemy.iDefensePower + (rand() % 3 + 1);
			if (damageEnemyBase < 0)
				damageEnemyBase = 0;

			//Receive damage
			this->OnDamage(u32(damageEnemyBase));
		}
	}
}

bool EnemyEntity::OnDamage(u32 amount)
{
	// Play damage sound
	gSoundManager->Play(SND_DAMAGE);

	if (fInvicibleTime > 0)
		return false;

	// Receive the damage
	this->SetLife(this->GetLife() - amount);

	if((int)this->GetLife() <= 0)
	{
		// Disable item
		this->pSprite->SetVisible(false);

		// Add body to a list to remove
		gPhysics->AddBodyToRemove(pBody);
		pBody = nullptr;
		bIsDead = true;
		gGui->SelectEnemy();
	}
	else
		fInvicibleTime = 3;

	return true;
}

String EnemyEntity::GetDisplayName() const
{
	return sEnemy.displayName;
}

void EnemyEntity::SetDisplayName(const String &displayName)
{
	gGui->SetEnemyName(displayName);
}

u32 EnemyEntity::GetLevel() const
{
	return sEnemy.iLevel;
}

void EnemyEntity::SetLevel(u32 level)
{
	sEnemy.iLevel = level;
	gGui->SetEnemyLevel(level);
}

u32 EnemyEntity::GetLife() const
{
	return sEnemy.iLife;
}

void EnemyEntity::SetLife(u32 life)
{
	sEnemy.iLife = life;
	gGui->SetEnemyLife(life, this->sEnemy.iLifeTotal);
}
