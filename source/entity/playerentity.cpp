#include <Box2D/Common/b2Math.h>
#include "playerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"
#include "../manager/guimanager.h"
#include "../gameflow.h"

#define PIX2M		0.01f
#define M2PIX		(1.0f / PIX2M)

ENTITY_CREATOR("Player", PlayerEntity)

PlayerEntity::PlayerEntity()
	: SpriteEntity("Player", "Player")
	, pBody(nullptr)
	, vPlayerVectorDirection()
	, eItem(ItemTypes::None)
	, iPreviousState(Idle)
	, iCurrentState(Idle)
	, fVelocity(0.0f)
	, fMove(0.0f)
	, fUpDownMove(0.0f)
	, fInvicibleTime(0.0f)
	, bKeyStillPressed(false)
	, bCanMove(true)
	, bIsActive(false)
	, bIsInputEnabled(true)
{
}

PlayerEntity::PlayerEntity(const char *className, const char *spriteName, bool bIsActive)
	: SpriteEntity(className, spriteName)
	, pBody(nullptr)
	, vPlayerVectorDirection()
	, eItem(ItemTypes::None)
	, iPreviousState(Idle)
	, iCurrentState(Idle)
	, fVelocity(0.0f)
	, fMove(0.0f)
	, fUpDownMove(0.0f)
	, fInvicibleTime(0.0f)
	, bKeyStillPressed(false)
	, bCanMove(true)
	, bIsActive(bIsActive)
	, bIsInputEnabled(true)
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

	b2Vec2 customSize(26, 26);

	pBody = gPhysics->CreateBody(pSprite, &customSize);
	pBody->SetFixedRotation(true);
	pBody->GetFixtureList()->SetUserData(this);

	pInput->AddKeyboardListener(this);
	//fVelocity = 2.0f;
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
	b2Vec2 vel = pBody->GetLinearVelocity();

	if (fInvicibleTime > 0)
	{
		pSprite->SetVisible(!pSprite->IsVisible());

		fInvicibleTime -= dt;
		if (fInvicibleTime <= 0)
		{
			pSprite->SetVisible(true);
			fInvicibleTime = 0;
			if (this->bIsActive)
			{
				this->bIsInputEnabled = true;
				this->StopPlayerMovement();
				SetState(Idle);
			}
		}
	}

	if (fMove != 0)
	{
		if (fVelocity > 0.0f)
		{
			if (fMove > 0)
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x + (PIX2M * 40), pBody->GetTransform().p.y), 0);
			else
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x - (PIX2M * 40), pBody->GetTransform().p.y), 0);

			fVelocity = 0.0f;
		}
		else
		{
			pBody->SetLinearVelocity(vel);
			if (!bKeyStillPressed)
			{
				bCanMove = true;
				fMove = 0;
			}
		}
	}

	if (fUpDownMove != 0)
	{
		if (fVelocity > 0.0f)
		{
			if (fUpDownMove > 0)
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y + (PIX2M * 40)), 0);
			else
				pBody->SetTransform(b2Vec2(pBody->GetTransform().p.x, pBody->GetTransform().p.y - (PIX2M * 40)), 0);

			fVelocity = 0.0f;
		}
		else
		{
			pBody->SetLinearVelocity(vel);
			if (!bKeyStillPressed)
			{
				bCanMove = true;
				fUpDownMove = 0;
			}
		}
	}

	if (iCurrentState == iPreviousState)
		return;

	if (iCurrentState == Run)
	{
		pSprite->SetAnimation("Run");
	}
	else
	{
		pSprite->SetAnimation("Idle");
	}

	iPreviousState = iCurrentState;
}

bool PlayerEntity::OnInputKeyboardPress(const EventInputKeyboard *ev)
{
	if (this->bIsActive && this->bIsInputEnabled)
	{
		Key k = ev->GetKey();

		if ((k == eKey::Up || k == eKey::W) && iCurrentState != Jump)
		{
			auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
			Vector3f movePos = Vector3f(pBody->GetTransform().p.x * M2PIX, ((pBody->GetTransform().p.y * M2PIX) - 40), -10 * M2PIX);
			auto tileId = map->GetTileAt(movePos);

			if (tileId != 3) // Wall
			{
				if (bCanMove)
				{
					SetState(Run);
					fUpDownMove = -1;
					fVelocity = 1.0f;
					bCanMove = false;
					bKeyStillPressed = true;
				}
			}
		}

		if (k == eKey::Left || k == eKey::A)
		{
			auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
			Vector3f movePos = Vector3f((pBody->GetTransform().p.x * M2PIX) - 40, (pBody->GetTransform().p.y * M2PIX), -10 * M2PIX);
			auto tileId = map->GetTileAt(movePos);

			if (tileId != 3) // Wall
			{
				if (bCanMove)
				{
					SetState(Run);
					fMove = -1;
					fVelocity = 1.0f;
					bCanMove = false;
					bKeyStillPressed = true;
				}
			}
		}

		if (k == eKey::Right || k == eKey::D)
		{
			auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
			Vector3f movePos = Vector3f((pBody->GetTransform().p.x * M2PIX) + 40, (pBody->GetTransform().p.y * M2PIX), -10 * M2PIX);
			auto tileId = map->GetTileAt(movePos);

			if (tileId != 3) // Wall
			{
				if (bCanMove)
				{
					SetState(Run);
					fMove = 1;
					fVelocity = 1.0f;
					bCanMove = false;
					bKeyStillPressed = true;
				}
			}
		}

		if (k == eKey::Down || k == eKey::S)
		{
			auto map = gGameScene->GetGameMap().GetLayerByName("Background")->AsTiled();
			Vector3f movePos = Vector3f(pBody->GetTransform().p.x * M2PIX, ((pBody->GetTransform().p.y * M2PIX) + 40 ), -10 * M2PIX);
			auto tileId = map->GetTileAt(movePos);

			if (tileId != 3) // Wall
			{
				if (bCanMove)
				{
					SetState(Run);
					fUpDownMove = 1;
					fVelocity = 1.0f;
					bCanMove = false;
					bKeyStillPressed = true;
				}
			}
		}
	}

	return true;
}

bool PlayerEntity::OnInputKeyboardRelease(const EventInputKeyboard *ev)
{
	if (this->bIsActive && this->bIsInputEnabled)
	{
		Key k = ev->GetKey();

		b2Vec2 vel = pBody->GetLinearVelocity();
		vel.x = 0;
		vel.y = 0;

		// Remove the directions
		if (k == eKey::Up|| k == eKey::W)
		{
			bKeyStillPressed = false;
		}

		if (k == eKey::Left|| k == eKey::A)
		{
			bKeyStillPressed = false;
		}

		if (k == eKey::Right|| k == eKey::D)
		{
			bKeyStillPressed = false;
		}

		if (k == eKey::Down|| k == eKey::S)
		{
			bKeyStillPressed = false;
		}

		if (fUpDownMove == 0 && fMove == 0)
		{
			SetState(Idle);
		}
	}

	return true;
}

void PlayerEntity::SetItem(ItemTypes::Enum item)
{
	eItem = item;
}

ItemTypes::Enum PlayerEntity::GetItem() const
{
	return eItem;
}

void PlayerEntity::SetIsActive(bool isActive)
{
	bIsActive = isActive;
}

void PlayerEntity::SetIsInputEnabled(bool isInputEnabled)
{
	bIsInputEnabled = isInputEnabled;
}

bool PlayerEntity::GetIsInputEnabled() const
{
	return bIsInputEnabled;
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

bool PlayerEntity::GetIsActive()
{
	return bIsActive;
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
	gGui->SetGold(gold);
}

u32 PlayerEntity::GetLife() const
{
	return sPlayer.iLife;
}
void PlayerEntity::SetLife(const u32 life)
{
	sPlayer.iLife = life;
	gGui->SetLife(life, this->sPlayer.iLifeTotal);
}
void PlayerEntity::RemoveLife()
{
	sPlayer.iLife--;
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
	gGui->SetStamina(stamina, this->sPlayer.iStaminaTotal);
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

bool PlayerEntity::OnDamage(const b2Vec2 vec2Push, u32 amount)
{
	// Play damage sound
	gSoundManager->Play(SND_DAMAGE);

	// Apply force to player
	pBody->ApplyForce(vec2Push, pBody->GetWorldCenter());

	// Create the ghost effect
	if (fInvicibleTime > 0)
		return false;

	fInvicibleTime = 1.0f;
	//pText->SetVisible(true);

	// Receive the damage
	u32 life = this->GetLife() - amount;

	if ((int)life > 1)
		this->SetLife(life);
	else
		gGameData->SetIsGameOver(true);

	return true;
}

void PlayerEntity::OnCollect(ItemTypes::Enum item, u32 amount)
{
	// Play collect sound
	gSoundManager->Play(SND_POWERUP);

	if(item == ItemTypes::HealthPotion
		&& (this->GetLife() + amount) < this->GetLifeTotal())
		this->SetLife(this->GetLife() + amount);

	if(item == ItemTypes::StaminaPotion
		&& (this->GetStamina() + amount) < this->GetStaminaTotal())
		this->SetStamina(this->GetStamina() + amount);

	if(item == ItemTypes::Gold)
		this->SetGold(this->GetGold() + amount);
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

void PlayerEntity::OnCollision(const CollisionEvent &event)
{
	if (event.GetType() == CollisionEventType::OnEnter)
	{
		Entity *other = event.GetOtherEntity();
		if (other != nullptr && other->GetClassName() == "Trigger")
		{
			gGameScene->UseKey(this->GiveKey());
		}
	}
}
