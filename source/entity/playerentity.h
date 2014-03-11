#ifndef _PLAYER_ENTITY_H
#define _PLAYER_ENTITY_H

#include <Box2D/Box2D.h>
#include "../defines.h"
#include "spriteentity.h"

namespace ItemTypes
{
	enum Enum
	{
		None,
		Text,
		HealthPotion,
		StaminaPotion,
		Gold
	};
}

class PlayerEntity: public SpriteEntity,
					public IEventInputKeyboardListener
{
	public:
		PlayerEntity();
		PlayerEntity(const char *className, const char *spriteName, bool bIsActive);

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

		void SetItem(ItemTypes::Enum item);
		ItemTypes::Enum GetItem() const;

		void Teleport(const b2Vec2 &position);
		bool OnDamage(const b2Vec2 vec2Push, u32 amount);
		void OnCollect(ItemTypes::Enum item, u32 amount);

		void StopPlayerMovement();
		void ChangePlayer();

		void SetIsActive(bool isActive);
		bool GetIsActive();

		void SetIsInputEnabled(bool isKeyboardEnabled);
		bool GetIsInputEnabled() const;

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

		u32 GetLifeTotal() const;
		void SetLifeTotal(u32);
		void RemoveLifeTotal();

		u32 GetStamina() const;
		void SetStamina(u32);
		void RemoveStamina();

		u32 GetStaminaTotal() const;
		void SetStaminaTotal(u32);
		void RemoveStaminaTotal();

		void Talk();
		void Mute();
		u32 GiveKey();
		void ReceiveKey(u32 key);

		u32 GetDefensePower() const;
		void SetDefensePower(u32 defensePower);

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
			bool bGameOver;
		} sPlayer;

	protected:
		b2Body *pBody;
		Sprite *pText;
		Vector3f vPlayerVectorDirection;

		ItemTypes::Enum eItem;

		enum eAnimationStates {Idle = 0, Run = 1, Jump = 2, Land = 3};
		s32 iPreviousState;
		s32 iCurrentState;

		f32 fVelocity;
		f32 fMove;
		f32 fUpDownMove;
		f32 fInvicibleTime;

		bool bIsActive;
		bool bIsInputEnabled;
		void SetState(int newState);
};

#endif
