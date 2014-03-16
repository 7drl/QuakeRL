#include <Box2D/Common/b2Math.h>
#include "playerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../manager/guimanager.h"
#include "../gameflow.h"
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

Vector3f PlayerEntity::GetPosition()
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
}

bool PlayerEntity::OnInputKeyboardPress(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();
	vLastPlayerPos = b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y);

	if ((k == eKey::Up || k == eKey::W) && iCurrentState != Jump)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		Vector3f movePos = Vector3f(ceil(pBody->GetTransform().p.x * M2PIX), ceil((pBody->GetTransform().p.y * M2PIX) - 40), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != 3) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y - (PIX2M * 40)), 0);
				bCanMove = false;
			}
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Left || k == eKey::A)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		Vector3f movePos = Vector3f(ceil((pBody->GetTransform().p.x * M2PIX) - 40), ceil(pBody->GetTransform().p.y * M2PIX), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != 3) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x - (PIX2M * 40), pBody->GetTransform().p.y), 0);
				bCanMove = false;
			}
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Right || k == eKey::D)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		Vector3f movePos = Vector3f(ceil((pBody->GetTransform().p.x * M2PIX) + 40), ceil(pBody->GetTransform().p.y * M2PIX), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != 3) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x + (PIX2M * 40), pBody->GetTransform().p.y), 0);
				bCanMove = false;
			}
		}

		if (pEnemyTarget != nullptr)
		{
			pEnemyTarget->FindPathToPlayer();
		}
	}

	if (k == eKey::Down || k == eKey::S)
	{
		auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
		Vector3f movePos = Vector3f(ceil(pBody->GetTransform().p.x * M2PIX), ceil((pBody->GetTransform().p.y * M2PIX) + 40 ), -10);
		auto tileId = map->GetTileAt(movePos);

		if (tileId != 3) // Wall
		{
			if (bCanMove)
			{
				SetState(Run);
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y + (PIX2M * 40)), 0);
				bCanMove = false;
			}
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

				// Do Damage to the enemy
				pEnemyTarget->ReceiveDamage(1, GetWeapon());
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

	b2Vec2 vel = pBody->GetLinearVelocity();
	vel.x = 0;
	vel.y = 0;

	// Remove the directions
	if (k == eKey::Up|| k == eKey::W)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(10);
		}
	}

	if (k == eKey::Left|| k == eKey::A)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(10);
		}
	}

	if (k == eKey::Right|| k == eKey::D)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(10);
		}
	}

	if (k == eKey::Down|| k == eKey::S)
	{
		bCanMove = true;

		//Receive damage
		if (pEnemyTarget != nullptr)
		{
			OnDamage(10);
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
	eWeapon = weapon;
	gGui->OnChangeWeapon(weapon, bWeponsGotten);
}

ItemTypes::Weapons PlayerEntity::GetWeapon() const
{
	return eWeapon;
}

void PlayerEntity::SetWeaponGot(ItemTypes::Weapons weapon)
{
	bWeponsGotten[weapon] = true;
}

bool* PlayerEntity::GetWeaponGot()
{
	return bWeponsGotten;
}

void PlayerEntity::StopPlayerMovement()
{
	b2Vec2 vel = pBody->GetLinearVelocity();
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

u32 PlayerEntity::GetLife() const
{
	return sPlayer.iLife;
}
void PlayerEntity::SetLife(const u32 life)
{
	sPlayer.iLife = life;
	gGui->SetLife(life);
}
void PlayerEntity::RemoveLife()
{
	sPlayer.iLife--;
}

u32 PlayerEntity::GetArmor() const
{
	return sPlayer.iArmor;
}

void PlayerEntity::SetArmor(u32 armor)
{
	sPlayer.iArmor = armor;
	gGui->SetArmor(armor);
}

void PlayerEntity::RemoveArmor()
{
	sPlayer.iArmor--;
}

u32 PlayerEntity::GetLifeTotal() const
{
	return sPlayer.iLifeTotal;
}
void PlayerEntity::SetLifeTotal(const u32 lifeTotal)
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
	gSoundManager->Play(SND_DAMAGE);

	fInvicibleTime = 0.6f;

	// Verify the armor of the player
	if(this->GetArmor() == 0)
	{
		// Receive the damage
		u32 life = this->GetLife() - amount;

		// Change avatar based on life
		gGui->OnDamageAvatar(life);

		if ((int)life > 1)
			this->SetLife(life);
		else
			gGameData->SetIsGameOver(true);
	}
	else
	{
		u32 armor = this->GetArmor() - amount;
		this->SetArmor(armor);
	}

	return true;
}

void PlayerEntity::OnCollect(u32 item, u32 amount)
{
	// Play collect sound
	gSoundManager->Play(SND_POWERUP);

	if(item == ItemTypes::Weapons::Rifle)
	{
		bWeponsGotten[ItemTypes::Weapons::Rifle] = true;
		uQuantityAmmoShells += 15;
		SetWeapon(ItemTypes::Weapons::Rifle);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::Rifle, bWeponsGotten);
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if(GetWeapon() == ItemTypes::Rifle)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::Shotgun)
	{
		bWeponsGotten[ItemTypes::Weapons::Shotgun] = true;
		uQuantityAmmoShells += 15;
		SetWeapon(ItemTypes::Weapons::Shotgun);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::Shotgun, bWeponsGotten);
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if(GetWeapon() == ItemTypes::Shotgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::Nailgun)
	{
		bWeponsGotten[ItemTypes::Weapons::Nailgun] = true;
		uQuantityAmmoNails += 25;
		SetWeapon(ItemTypes::Weapons::Nailgun);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::Nailgun, bWeponsGotten);
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if(GetWeapon() == ItemTypes::Nailgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::HeavyNailgun)
	{
		bWeponsGotten[ItemTypes::Weapons::HeavyNailgun] = true;
		uQuantityAmmoNails += 25;
		SetWeapon(ItemTypes::Weapons::HeavyNailgun);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::HeavyNailgun, bWeponsGotten);
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if(GetWeapon() == ItemTypes::HeavyNailgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::GrenadeLauncher)
	{
		bWeponsGotten[ItemTypes::Weapons::GrenadeLauncher] = true;
		uQuantityAmmoRockets += 5;
		SetWeapon(ItemTypes::Weapons::GrenadeLauncher);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::GrenadeLauncher, bWeponsGotten);
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if(GetWeapon() == ItemTypes::GrenadeLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::RocketLauncher)
	{
		bWeponsGotten[ItemTypes::Weapons::RocketLauncher] = true;
		uQuantityAmmoRockets += 5;
		SetWeapon(ItemTypes::Weapons::RocketLauncher);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::RocketLauncher, bWeponsGotten);
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if(GetWeapon() == ItemTypes::RocketLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::Weapons::Shockgun)
	{
		bWeponsGotten[ItemTypes::Weapons::Shockgun] = true;
		uQuantityAmmoShock += 5;
		SetWeapon(ItemTypes::Weapons::Shockgun);

		// UI
		gGui->OnChangeWeapon(ItemTypes::Weapons::Shockgun, bWeponsGotten);
		gGui->SetAmmoCells(uQuantityAmmoShock);

		if(GetWeapon() == ItemTypes::Shockgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::HealthPotion)
	{
		//TODO play health pickup item
		//gSoundManager->Play(SND_POWERUP);

		SetLife(amount);
		gGui->OnDamageAvatar(amount);
	}

	if(item == ItemTypes::LightArmor)
	{
		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if(item == ItemTypes::MediumArmor)
	{
		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if(item == ItemTypes::HeavyArmor)
	{
		SetArmor(amount);
		gGui->OnGetArmor(amount);
	}

	if(item == ItemTypes::ShellsAmmo)
	{
		uQuantityAmmoShells += amount;
		gGui->SetAmmoShells(uQuantityAmmoShells);

		if(GetWeapon() == ItemTypes::Rifle || GetWeapon() == ItemTypes::Shotgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::NailsAmmo)
	{
		uQuantityAmmoNails += amount;
		gGui->SetAmmoNails(uQuantityAmmoNails);

		if(GetWeapon() == ItemTypes::Nailgun || GetWeapon() == ItemTypes::HeavyNailgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::RocketsAmmo)
	{
		uQuantityAmmoRockets += amount;
		gGui->SetAmmoRockets(uQuantityAmmoRockets);

		if(GetWeapon() == ItemTypes::GrenadeLauncher
				|| GetWeapon() == ItemTypes::RocketLauncher)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
	}

	if(item == ItemTypes::ShockAmmo)
	{
		uQuantityAmmoShock += amount;
		gGui->SetAmmoCells(uQuantityAmmoShock);

		if(GetWeapon() == ItemTypes::Shockgun)
			gGui->SetAmmoSelected(uQuantityAmmoShells);
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
}

void PlayerEntity::LoadPlayerDamageAnimation()
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

bool PlayerEntity::DecreaseAmmo()
{
	if(GetWeapon() == ItemTypes::Rifle || GetWeapon() == ItemTypes::Shotgun)
	{
		if(uQuantityAmmoShells)
		{
			uQuantityAmmoShells--;
			gGui->SetAmmoShells(uQuantityAmmoShells);
			gGui->SetAmmoSelected(uQuantityAmmoShells);
			return true;
		}
		else
			return false;
	}
	else if(GetWeapon() == ItemTypes::Nailgun || GetWeapon() == ItemTypes::HeavyNailgun)
	{
		if(uQuantityAmmoNails)
		{
			uQuantityAmmoNails--;
			gGui->SetAmmoNails(uQuantityAmmoNails);
			gGui->SetAmmoSelected(uQuantityAmmoShells);
			return true;
		}
		else
			return false;
	}
	else if(GetWeapon() == ItemTypes::GrenadeLauncher
			|| GetWeapon() == ItemTypes::RocketLauncher)
	{
		if(uQuantityAmmoRockets)
		{
			uQuantityAmmoRockets--;
			gGui->SetAmmoRockets(uQuantityAmmoRockets);
			gGui->SetAmmoSelected(uQuantityAmmoShells);
			return true;
		}
		else
			return false;
	}
	else if(GetWeapon() == ItemTypes::Shockgun)
	{
		if(uQuantityAmmoShock)
		{
			uQuantityAmmoShock--;
			gGui->SetAmmoCells(uQuantityAmmoShock);
			gGui->SetAmmoSelected(uQuantityAmmoShells);
			return true;
		}
		else
			return false;
	}
	// Axe damage
	else
		return true;
}

void PlayerEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Entity *other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "Trigger")
		{
			gGameScene->ChangeLevel();
		}
	}
}

