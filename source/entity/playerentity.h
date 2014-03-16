#ifndef _PLAYER_ENTITY_H
#define _PLAYER_ENTITY_H

#include <Box2D/Box2D.h>
#include "../defines.h"
#include "spriteentity.h"
#include "weaponentity.h"

namespace ItemTypes
{

	enum Weapons
	{
		Empty				= 0,
		Axe					= 1,
		Rifle				= 2,
		Shotgun				= 3,
		Nailgun				= 4,
		HeavyNailgun		= 5,
		GrenadeLauncher		= 6,
		RocketLauncher		= 7,
		Shockgun			= 8
	};

	enum Consumables
	{
		None				= 9,
		HealthPotion		= 10,
		LightArmor			= 11,
		MediumArmor			= 12,
		HeavyArmor			= 13,
		ShellsAmmo			= 14,
		NailsAmmo			= 15,
		RocketsAmmo			= 16,
		ShockAmmo			= 17
	};
}

// Forward declaration of EnemyEntity.
class EnemyEntity;

class PlayerEntity: public SpriteEntity,
					public IEventInputKeyboardListener
{
	public:
		PlayerEntity();
		PlayerEntity(const char *className, const char *spriteName);

		virtual ~PlayerEntity();

		void Load(MetadataObject &metadata, SceneNode *sprites);
		void Update(f32 dt);

		// IEventInputKeyboardListener
		bool OnInputKeyboardPress(const EventInputKeyboard *ev) override;

		// IEventInputKeyboardListener
		bool OnInputKeyboardRelease(const EventInputKeyboard *ev) override;

		void OnCollision(const CollisionEvent &event);

		Vector3f GetPosition();
		Sprite *GetSprite() const;
		b2Vec2 GetBodyPosition() const;

		void SetItem(ItemTypes::Consumables item);
		ItemTypes::Consumables GetItem() const;

		void SetWeapon(ItemTypes::Weapons weapon);
		ItemTypes::Weapons GetWeapon() const;

		void SetWeaponGot(ItemTypes::Weapons weapon);
		bool *GetWeaponGot();

		void Teleport(const b2Vec2 &position);
		bool OnDamage(u32 amount);
		void OnCollect(u32 item, u32 amount);

		void StopPlayerMovement();
		void ChangePlayer();

		String GetDisplayName() const;

		u32 GetLevel() const;
		void SetLevel(u32);

		u32 GetXP() const;
		void SetXP(u32);

		u32 GetAttackPower() const;
		void SetAttackPower(u32);

		u32 GetGold() const;
		void SetGold(u32);

		u32 GetLife() const;
		void SetLife(u32);
		void RemoveLife();

		u32 GetArmor() const;
		void SetArmor(u32);
		void RemoveArmor();

		u32 GetLifeTotal() const;
		void SetLifeTotal(u32);
		void RemoveLifeTotal();

		u32 GetStamina() const;
		void SetStamina(u32);
		void RemoveStamina();

		u32 GetStaminaTotal() const;
		void SetStaminaTotal(u32);
		void RemoveStaminaTotal();

		u32 GiveKey();
		void ReceiveKey(u32 key);

		u32 GetDefensePower() const;
		void SetDefensePower(u32 defensePower);

		void SetEnemyTarget(EnemyEntity *enemyTarget);
		EnemyEntity *GetEnemyTarget();

		void PlayShotSound();
		void LoadPlayerDamageAnimation();
		void LoadPlayerDamageSound();
		bool DecreaseAmmo();
		u32 CalculateDamage();

		struct PlayerData
		{
			String displayName;
			u32 iLevel;
			u32 iXP;
			u32 iAttackPower;
			u32 iDefensePower;
			u32 iGold;
			u32 iLife;
			u32 iLifeTotal;
			u32 iStamina;
			u32 iStaminaTotal;
			u32 iKey;
			u32 iArmor;
			bool bGameOver;
		} sPlayer;

	protected:
		b2Body *pBody;
		b2Vec2 vLastPlayerPos;
		Vector3f vPlayerVectorDirection;

		ItemTypes::Consumables eItem;
		ItemTypes::Weapons eWeapon;

		enum eAnimationStates {Idle = 0, Run = 1, Jump = 2, Land = 3};
		s32 iPreviousState;
		s32 iCurrentState;

		// Movement members
		f32 fVelocity;
		f32 fMove;
		f32 fUpDownMove;
		f32 fInvicibleTime;

		// Ammo for the player
		u32 uQuantityAmmoShells;
		u32 uQuantityAmmoNails;
		u32 uQuantityAmmoRockets;
		u32 uQuantityAmmoShock;

		bool bWeponsGotten[9] = {false, true, true , false, false, false, false, false, false};

		// Maybe in the future we could put that in a list, Then the player could manage weapons in an inventory
		WeaponEntity cRifleWeapon;
		WeaponEntity cShotgunWeapon;
		WeaponEntity cNailgunWeapon;
		WeaponEntity cHeavyNailgunWeapon;
		WeaponEntity cGrenadeWeapon;
		WeaponEntity cRocketWeapon;
		WeaponEntity cShockWeapon;

		// Enemy Target
		EnemyEntity *pEnemyTarget;

		bool bCanMove;
		void SetState(int newState);
};

#endif
