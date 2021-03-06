#include <Box2D/Common/b2Math.h>
#include "playerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../manager/guimanager.h"
#include "../gameflow.h"
#include "../manager/proceduralmanager.h"
#include <math.h>

#define PIX2M		0.01f
#define M2PIX		(1.0f / PIX2M)

ENTITY_CREATOR("Player", PlayerEntity)

PlayerEntity::PlayerEntity()
	: SpriteEntity("Player", "Player")
	, pBody(nullptr)
	, vLastPlayerPos(b2Vec2(0,0))
	, vPlayerVectorDirection()
	, eItem(ItemTypes::Consumables::None)
	, eWeapon(ItemTypes::Weapons::Rifle)
	, iPreviousState(Idle)
	, iCurrentState(Idle)
	, fVelocity(0.0f)
	, fMove(0.0f)
	, fUpDownMove(0.0f)
	, fInvicibleTime(0.0f)
	, uQuantityAmmoShells(25)
	, uQuantityAmmoNails(0)
	, uQuantityAmmoRockets(0)
	, uQuantityAmmoShock(0)
	, pEnemyTarget(nullptr)
	, bCanMove(true)
{
	bWeponsGotten[0] = false;
	bWeponsGotten[1] = true;
	bWeponsGotten[2] = true;
	bWeponsGotten[3] = false;
	bWeponsGotten[4] = false;
	bWeponsGotten[5] = false;
	bWeponsGotten[6] = false;
	bWeponsGotten[7] = false;
	bWeponsGotten[8] = false;
}

PlayerEntity::PlayerEntity(const char *className, const char *spriteName)
	: SpriteEntity(className, spriteName)
	, pBody(nullptr)
	, vLastPlayerPos(b2Vec2(0,0))
	, vPlayerVectorDirection()
	, eItem(ItemTypes::Consumables::None)
	, eWeapon(ItemTypes::Weapons::Rifle)
	, iPreviousState(Idle)
	, iCurrentState(Idle)
	, fVelocity(0.0f)
	, fMove(0.0f)
	, fUpDownMove(0.0f)
	, fInvicibleTime(0.0f)
	, uQuantityAmmoShells(25)
	, uQuantityAmmoNails(0)
	, uQuantityAmmoRockets(0)
	, uQuantityAmmoShock(0)
	, pEnemyTarget(nullptr)
	, bCanMove(true)
{
	bWeponsGotten[0] = false;
	bWeponsGotten[1] = true;
	bWeponsGotten[2] = true;
	bWeponsGotten[3] = false;
	bWeponsGotten[4] = false;
	bWeponsGotten[5] = false;
	bWeponsGotten[6] = false;
	bWeponsGotten[7] = false;
	bWeponsGotten[8] = false;
}

PlayerEntity::~PlayerEntity()
{
	pInput->RemoveKeyboardListener(this);
	gPhysics->DestroyBody(pBody);
	pBody = nullptr;
}

void PlayerEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	pSprite->SetZ(-10);

	pBody = gPhysics->CreateBody(pSprite);
	pBody->SetFixedRotation(true);
	pBody->GetFixtureList()->SetUserData(this);

	vLastPlayerPos = b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y);

	pInput->AddKeyboardListener(this);
	vPlayerVectorDirection = VECTOR_ZERO;
}

vec3 PlayerEntity::GetPosition()
{
	return pSprite->GetPosition();
}

b2Vec2 PlayerEntity::GetBodyPosition() const
{
	return pBody->GetPosition();
}

Sprite *PlayerEntity::GetSprite() const
{
	return pSprite;
}

void PlayerEntity::Teleport(const b2Vec2 &position)
{
	pBody->SetTransform(position, pBody->GetAngle());

	fMove= 0;
	fUpDownMove= 0;
	this->SetState(Idle);

	gSoundManager->Play(SND_TELEPORT);
}

void PlayerEntity::Update(f32 dt)
{
	if (fInvicibleTime > 0)
	{

		fInvicibleTime -= dt;
		if (fInvicibleTime <= 0)
		{
			fInvicibleTime = 0;
			SetState(Idle);
			pSprite->SetAnimation("Idle");

			// Add the ability to move again
			bCanMove = true;
		}
	}

	// UGLY - This is used to by pass a problem with colliders
	auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
	auto curPos = vec3(ceil(pBody->GetTransform().p.x * M2PIX), ceil((pBody->GetTransform().p.y * M2PIX)), -10);
	auto tileId = map->GetTileAt(curPos);

	if (tileId == ProceduralManager::eTiles::tileDownStairs)
	{
		gGameScene->ChangeLevel();
	}
}

bool PlayerEntity::OnInputKeyboardPress(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();
	vLastPlayerPos = b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y);

	if ((k == eKey::Up || k == eKey::W) && iCurrentState != Jump)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		auto movePos = vec3(ceil(pBody->GetTransform().p.x * M2PIX), ceil((pBody->GetTransform().p.y * M2PIX) - 40), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != ProceduralManager::eTiles::tileStoneWall) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y - (PIX2M * 40)), 0);
				bCanMove = false;
			}
		}
		else
		{
			gSoundManager->Play(SND_STUCK_STEP);
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Left || k == eKey::A)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		auto movePos = vec3(ceil((pBody->GetTransform().p.x * M2PIX) - 40), ceil(pBody->GetTransform().p.y * M2PIX), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != ProceduralManager::eTiles::tileStoneWall) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x - (PIX2M * 40), pBody->GetTransform().p.y), 0);
				bCanMove = false;
			}
		}
		else
		{
			gSoundManager->Play(SND_STUCK_STEP);
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Right || k == eKey::D)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		auto movePos = vec3(ceil((pBody->GetTransform().p.x * M2PIX) + 40), ceil(pBody->GetTransform().p.y * M2PIX), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != ProceduralManager::eTiles::tileStoneWall) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x + (PIX2M * 40), pBody->GetTransform().p.y), 0);
				bCanMove = false;
			}
		}
		else
		{
			gSoundManager->Play(SND_STUCK_STEP);
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Down || k == eKey::S)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		auto movePos = vec3(ceil(pBody->GetTransform().p.x * M2PIX), ceil((pBody->GetTransform().p.y * M2PIX) + 40 ), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != ProceduralManager::eTiles::tileStoneWall) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y + (PIX2M * 40)), 0);
				bCanMove = false;
			}
		}
		else
		{
			gSoundManager->Play(SND_STUCK_STEP);
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::LeftCtrl || k == eKey::RightCtrl)
	{
		if (pEnemyTarget != nullptr)
		{
			// Verify if player has ammo
			if(DecreaseAmmo())
			{
				// Play shot sound
				PlayShotSound();

				// Calculate Damage
				u32 damage = CalculateDamage();

				// Do Damage to the enemy
				pEnemyTarget->ReceiveDamage(damage, GetWeapon());
			}

		}
	}

	if (k == eKey::Digit1)
	{
		SetWeapon(ItemTypes::Weapons::Axe);
	}

	if (k == eKey::Digit2)
	{
		SetWeapon(ItemTypes::Weapons::Rifle);
	}

	if (k == eKey::Digit3)
	{
		SetWeapon(ItemTypes::Weapons::Shotgun);
	}

	if (k == eKey::Digit4)
	{
		SetWeapon(ItemTypes::Weapons::Nailgun);
	}

	if (k == eKey::Digit5)
	{
		SetWeapon(ItemTypes::Weapons::HeavyNailgun);
	}

	if (k == eKey::Digit6)
	{
		SetWeapon(ItemTypes::Weapons::GrenadeLauncher);
	}

	if (k == eKey::Digit7)
	{
		SetWeapon(ItemTypes::Weapons::RocketLauncher);
	}

	if (k == eKey::Digit8)
	{
		SetWeapon(ItemTypes::Weapons::Shockgun);
	}

	return true;
}

bool PlayerEntity::OnInputKeyboardRelease(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();

	auto vel = pBody->GetLinearVelocity();
	vel.x = 0;
	vel.y = 0;

	// Remove the directions
	if (k == eKey::Up|| k == eKey::W)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(pEnemyTarget->sEnemy.iAttackPower);
		}
	}

	if (k == eKey::Left|| k == eKey::A)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(pEnemyTarget->sEnemy.iAttackPower);
		}
	}

	if (k == eKey::Right|| k == eKey::D)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(pEnemyTarget->sEnemy.iAttackPower);
		}
	}

	if (k == eKey::Down|| k == eKey::S)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr && !pEnemyTarget->IsDead())
		{
			OnDamage(pEnemyTarget->sEnemy.iAttackPower);
		}
	}

	if (fUpDownMove == 0 && fMove == 0)
	{
		SetState(Idle);
	}

	return true;
}

void PlayerEntity::SetItem(ItemTypes::Consumables item)
{
	eItem = item;
}

ItemTypes::Consumables PlayerEntity::GetItem() const
{
	return eItem;
}

void PlayerEntity::SetWeapon(ItemTypes::Weapons weapon)
{

	if (weapon == ItemTypes::Rifle && bWeponsGotten[ItemTypes::Weapons::Rifle])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoShells);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if (weapon == ItemTypes::Shotgun && bWeponsGotten[ItemTypes::Weapons::Shotgun])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoShells);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if (weapon == ItemTypes::Nailgun && bWeponsGotten[ItemTypes::Weapons::Nailgun])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoNails);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if (weapon == ItemTypes::HeavyNailgun && bWeponsGotten[ItemTypes::Weapons::HeavyNailgun])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoNails);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if (weapon == ItemTypes::GrenadeLauncher && bWeponsGotten[ItemTypes::Weapons::GrenadeLauncher])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoRockets);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if (weapon == ItemTypes::RocketLauncher && bWeponsGotten[ItemTypes::Weapons::RocketLauncher])
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoRockets);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}
	else if((weapon == ItemTypes::Shockgun && bWeponsGotten[ItemTypes::Weapons::Shockgun]))
	{
		eWeapon = weapon;
		gGui->SetAmmoSelected(uQuantityAmmoShock);
		gGui->OnChangeWeapon(weapon, bWeponsGotten);
	}

}

ItemTypes::Weapons PlayerEntity::GetWeapon() const
{
	return eWeapon;
}

void PlayerEntity::SetWeaponGot(ItemTypes::Weapons weapon)
{
	bWeponsGotten[weapon] = true;
}

bool *PlayerEntity::GetWeaponGot()
{
	return bWeponsGotten;
}

void PlayerEntity::StopPlayerMovement()
{
	auto vel = pBody->GetLinearVelocity();
	vel.x = 0;
	vel.y = 0;

	pBody->SetLinearVelocity(vel);
	fUpDownMove = 0;
	fMove = 0;
}

void PlayerEntity::SetState(int newState)
{
	iPreviousState = iCurrentState;
	iCurrentState = newState;
}

String PlayerEntity::GetDisplayName() const
{
	return sPlayer.displayName;
}

u32 PlayerEntity::GetLevel() const
{
	return sPlayer.iLevel;
}
void PlayerEntity::SetLevel(const u32 level)
{
	sPlayer.iLevel = level;
}

u32 PlayerEntity::GetXP() const
{
	return sPlayer.iXP;
}
void PlayerEntity::SetXP(const u32 xp)
{
	sPlayer.iXP = xp;
}

u32 PlayerEntity::GetAttackPower() const
{
	return sPlayer.iAttackPower;
}
void PlayerEntity::SetAttackPower(const u32 attackPower)
{
	sPlayer.iAttackPower = attackPower;
}

u32 PlayerEntity::GetGold() const
{
	return sPlayer.iGold;
}
void PlayerEntity::SetGold(const u32 gold)
{
	sPlayer.iGold = gold;
	//gGui->SetGold(gold);
}

s32 PlayerEntity::GetLife() const
{
	return sPlayer.iLife;
}
void PlayerEntity::SetLife(const s32 life)
{
	sPlayer.iLife = life;
	gGui->SetLife(life);
}
void PlayerEntity::RemoveLife()
{
	sPlayer.iLife--;
}

s32 PlayerEntity::GetArmor() const
{
	return sPlayer.iArmor;
}

void PlayerEntity::SetArmor(s32 armor)
{
	sPlayer.iArmor = armor;
	gGui->SetArmor(armor);
}

void PlayerEntity::RemoveArmor()
{
	sPlayer.iArmor--;
}

s32 PlayerEntity::GetLifeTotal() const
{
	return sPlayer.iLifeTotal;
}

void PlayerEntity::SetLifeTotal(const s32 lifeTotal)
{
	sPlayer.iLifeTotal = lifeTotal;
}

void PlayerEntity::RemoveLifeTotal()
{
	sPlayer.iLifeTotal--;
}

u32 PlayerEntity::GetStamina() const
{
	return sPlayer.iStamina;
}

void PlayerEntity::SetStamina(const u32 stamina)
{
	sPlayer.iStamina = stamina;
	//gGui->SetStamina(stamina, this->sPlayer.iStaminaTotal);
}

void PlayerEntity::RemoveStamina()
{
	sPlayer.iStamina--;
}

u32 PlayerEntity::GetStaminaTotal() const
{
	return sPlayer.iStaminaTotal;
}

void PlayerEntity::SetStaminaTotal(const u32 staminaTotal)
{
	sPlayer.iStaminaTotal = staminaTotal;
}

void PlayerEntity::RemoveStaminaTotal()
{
	sPlayer.iStaminaTotal--;
}

u32 PlayerEntity::GetDefensePower() const
{
	return sPlayer.iDefensePower;
}

void PlayerEntity::SetDefensePower(u32 defensePower)
{
	sPlayer.iDefensePower = defensePower;
}

void PlayerEntity::SetEnemyTarget(EnemyEntity *enemyTarget)
{
	pEnemyTarget = enemyTarget;
}

EnemyEntity *PlayerEntity::GetEnemyTarget()
{
	return pEnemyTarget;
}

bool PlayerEntity::OnDamage(u32 amount)
{
	// Move Player to last tile position
	//pBody->SetTransform(vLastPlayerPos, 0);

	// Load player damage animation
	LoadPlayerDamageAnimation();

	// Create the ghost effect
	if (fInvicibleTime > 0)
		return false;

	// Play damage sound
	LoadPlayerDamageSound();

	fInvicibleTime = 0.6f;

	// Verify the armor of the player
	if(this->GetArmor() <= 0)
	{
		// Receive the damage
		auto life = this->GetLife() - amount;

		// Change avatar based on life
		gGui->OnDamageAvatar(life);

		if (life > 1)
		{
			this->SetLife(life);
		}
		else
		{
			gSoundManager->Play(SND_DEATH);
			gGameData->SetIsGameOver(true);
		}
	}
	else
	{
		auto armor = this->GetArmor() - amount;
		this->SetArmor(armor);
	}

	return true;
}

void PlayerEntity::OnCollect(u32 item, u32 amount)
{
	if (item == ItemTypes::Weapons::Rifle)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::Rifle] = true;
		uQuantityAmmoShells += 15;

		// UI
		SetWeapon(ItemTypes::Weapons::Rifle);
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if (GetWeapon() == ItemTypes::Rifle)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if (item == ItemTypes::Weapons::Shotgun)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::Shotgun] = true;
		uQuantityAmmoShells += 15;

		// UI
		SetWeapon(ItemTypes::Weapons::Shotgun);
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if (GetWeapon() == ItemTypes::Shotgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if (item == ItemTypes::Weapons::Nailgun)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::Nailgun] = true;
		uQuantityAmmoNails += 25;

		// UI
		SetWeapon(ItemTypes::Weapons::Nailgun);
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if (GetWeapon() == ItemTypes::Nailgun)
			gGui->SetAmmoSelected(uQuantityAmmoNails);
	}

	if (item == ItemTypes::Weapons::HeavyNailgun)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::HeavyNailgun] = true;
		uQuantityAmmoNails += 25;

		// UI
		SetWeapon(ItemTypes::Weapons::HeavyNailgun);
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if (GetWeapon() == ItemTypes::HeavyNailgun)
			gGui->SetAmmoSelected(uQuantityAmmoNails);
	}

	if (item == ItemTypes::Weapons::GrenadeLauncher)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::GrenadeLauncher] = true;
		uQuantityAmmoRockets += 5;

		// UI
		SetWeapon(ItemTypes::Weapons::GrenadeLauncher);
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if (GetWeapon() == ItemTypes::GrenadeLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoRockets);
	}

	if (item == ItemTypes::Weapons::RocketLauncher)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::RocketLauncher] = true;
		uQuantityAmmoRockets += 5;

		// UI
		SetWeapon(ItemTypes::Weapons::RocketLauncher);
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if (GetWeapon() == ItemTypes::RocketLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoRockets);
	}

	if (item == ItemTypes::Weapons::Shockgun)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_GUN);

		bWeponsGotten[ItemTypes::Weapons::Shockgun] = true;
		uQuantityAmmoShock += 5;

		// UI
		SetWeapon(ItemTypes::Weapons::Shockgun);
		gGui->SetAmmoCells(uQuantityAmmoShock);

		if (GetWeapon() == ItemTypes::Shockgun)
			gGui->SetAmmoSelected(uQuantityAmmoShock);
	}

	if (item == ItemTypes::HealthPotion)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_HEALTH);

		auto life = GetLife() + amount;
		SetLife(life <= 100 ? life : 100);
		gGui->OnDamageAvatar(life);
	}

	if (item == ItemTypes::LightArmor)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_ARMOR);

		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if (item == ItemTypes::MediumArmor)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_ARMOR);

		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if (item == ItemTypes::HeavyArmor)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_ARMOR);

		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if (item == ItemTypes::ShellsAmmo)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_AMMO);

		uQuantityAmmoShells += amount;
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if (GetWeapon() == ItemTypes::Rifle || GetWeapon() == ItemTypes::Shotgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if (item == ItemTypes::NailsAmmo)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_AMMO);

		uQuantityAmmoNails += amount;
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if(GetWeapon() == ItemTypes::Nailgun || GetWeapon() == ItemTypes::HeavyNailgun)
			gGui->SetAmmoSelected(uQuantityAmmoNails);
	}

	if (item == ItemTypes::RocketsAmmo)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_AMMO);

		uQuantityAmmoRockets += amount;
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if(GetWeapon() == ItemTypes::GrenadeLauncher
				|| GetWeapon() == ItemTypes::RocketLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoRockets);
	}

	if (item == ItemTypes::ShockAmmo)
	{
		// Play sound
		gSoundManager->Play(SND_PICKUP_AMMO);

		uQuantityAmmoShock += amount;
		gGui->SetAmmoCells(uQuantityAmmoShock);

		if(GetWeapon() == ItemTypes::Shockgun)
			gGui->SetAmmoSelected(uQuantityAmmoShock);
	}
}

u32 PlayerEntity::GiveKey()
{
	auto key = sPlayer.iKey;
	sPlayer.iKey = 0;
	return key;
}

void PlayerEntity::ReceiveKey(u32 key)
{
	sPlayer.iKey = key;
}

void PlayerEntity::PlayShotSound()
{
	if (GetWeapon() == ItemTypes::Weapons::Rifle)
	{
		gSoundManager->Play(SND_RIFLE_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::Shotgun)
	{
		gSoundManager->Play(SND_SHOTGUN_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::Nailgun)
	{
		gSoundManager->Play(SND_NAILGUN_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::HeavyNailgun)
	{
		gSoundManager->Play(SND_HEAVY_NAILGUN_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::GrenadeLauncher)
	{
		gSoundManager->Play(SND_EXPLOSION_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::RocketLauncher)
	{
		gSoundManager->Play(SND_EXPLOSION_SHOT);
	}
	else if (GetWeapon() == ItemTypes::Weapons::Shockgun)
	{
		gSoundManager->Play(SND_SHOCKGUN_SHOT);
	}

}

void PlayerEntity::LoadPlayerDamageAnimation()
{
	if (pEnemyTarget != nullptr)
	{
		if (pEnemyTarget->sEnemy.iEnemyId == 1)
			pSprite->SetAnimation("Blood");
		else if (pEnemyTarget->sEnemy.iEnemyId == 2)
			pSprite->SetAnimation("Explosion");
		else if (pEnemyTarget->sEnemy.iEnemyId == 3)
			pSprite->SetAnimation("Blood");
		else
			pSprite->SetAnimation("Blood");
	}
}

void PlayerEntity::LoadPlayerDamageSound()
{
	if (pEnemyTarget != nullptr)
	{
		if (pEnemyTarget->sEnemy.iEnemyId == 1)
			gSoundManager->Play(SND_RIFLE_SHOT);
		else if (pEnemyTarget->sEnemy.iEnemyId == 2)
			gSoundManager->Play(SND_EXPLOSION_SHOT);
		else if (pEnemyTarget->sEnemy.iEnemyId == 3)
			gSoundManager->Play(SND_RIFLE_SHOT);
		else
			gSoundManager->Play(SND_RIFLE_SHOT);
	}
}

bool PlayerEntity::DecreaseAmmo()
{
	if (GetWeapon() == ItemTypes::Rifle || GetWeapon() == ItemTypes::Shotgun)
	{
		if (uQuantityAmmoShells)
		{
			uQuantityAmmoShells--;
			gGui->SetAmmoShells(uQuantityAmmoShells);
			gGui->SetAmmoSelected(uQuantityAmmoShells);
			return true;
		}
		return false;
	}
	else if (GetWeapon() == ItemTypes::Nailgun || GetWeapon() == ItemTypes::HeavyNailgun)
	{
		if (uQuantityAmmoNails)
		{
			uQuantityAmmoNails--;
			gGui->SetAmmoNails(uQuantityAmmoNails);
			gGui->SetAmmoSelected(uQuantityAmmoNails);
			return true;
		}
		return false;
	}
	else if (GetWeapon() == ItemTypes::GrenadeLauncher || GetWeapon() == ItemTypes::RocketLauncher)
	{
		if (uQuantityAmmoRockets)
		{
			uQuantityAmmoRockets--;
			gGui->SetAmmoRockets(uQuantityAmmoRockets);
			gGui->SetAmmoSelected(uQuantityAmmoRockets);
			return true;
		}
		return false;
	}
	else if (GetWeapon() == ItemTypes::Shockgun)
	{
		if (uQuantityAmmoShock)
		{
			uQuantityAmmoShock--;
			gGui->SetAmmoCells(uQuantityAmmoShock);
			gGui->SetAmmoSelected(uQuantityAmmoShock);
			return true;
		}
		return false;
	}
	// Axe damage
	return true;
}

u32 PlayerEntity::CalculateDamage()
{
	if (GetWeapon() == ItemTypes::Rifle)
	{
		return 2;
	}
	else if (GetWeapon() == ItemTypes::Shotgun)
	{
		return 3;
	}
	else if (GetWeapon() == ItemTypes::Nailgun)
	{
		return 4;
	}
	else if (GetWeapon() == ItemTypes::HeavyNailgun)
	{
		return 5;
	}
	else if (GetWeapon() == ItemTypes::GrenadeLauncher)
	{
		return 8;
	}
	else if (GetWeapon() == ItemTypes::RocketLauncher)
	{
		return 8;
	}
	else if (GetWeapon() == ItemTypes::Shockgun)
	{
		return 9;
	}
	// Axe damage
	return 1;
}

void PlayerEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		auto other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "Trigger")
			gGameScene->ChangeLevel();
	}

	if (event.GetType() == CollisionEventType::OnLeave)
	{
		auto other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "Trigger")
			gGameScene->ChangeLevel();
	}
}
