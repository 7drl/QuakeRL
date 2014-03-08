#include "pessimistplayerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("PessimistPlayer", PessimistPlayerEntity)

PessimistPlayerEntity::PessimistPlayerEntity()
	: PlayerEntity("PessimistPlayer", "PessimistPlayer", false)
{
	fVelocity = 0.7f;

	sPlayer.iKey = 3;
	sPlayer.displayName = "Pessimist";
	sPlayer.iLevel = 1;
	sPlayer.iXP = 200;
	sPlayer.iAttackPower = 5;
	sPlayer.iDefensePower = 15;
	sPlayer.iGold = 0;
	sPlayer.iLife = 12;
	sPlayer.iLifeTotal = 12;
	sPlayer.iStamina = 20;
	sPlayer.iStaminaTotal = 20;
}

