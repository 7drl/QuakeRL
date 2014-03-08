#include "realistplayerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("RealistPlayer", RealistPlayerEntity)

RealistPlayerEntity::RealistPlayerEntity()
	: PlayerEntity("RealistPlayer", "RealistPlayer", false)
{
	fVelocity = 1.5f;

	sPlayer.iKey = 2;
	sPlayer.displayName = "Realist";
	sPlayer.iLevel = 1;
	sPlayer.iXP = 100;
	sPlayer.iAttackPower = 10;
	sPlayer.iDefensePower = 10;
	sPlayer.iGold = 0;
	sPlayer.iLife = 22;
	sPlayer.iLifeTotal = 22;
	sPlayer.iStamina = 10;
	sPlayer.iStaminaTotal = 10;
}
