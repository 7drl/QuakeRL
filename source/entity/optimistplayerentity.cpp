#include "optimistplayerentity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"

ENTITY_CREATOR("OptimistPlayer", OptimistPlayerEntity)

OptimistPlayerEntity::OptimistPlayerEntity()
	: PlayerEntity("OptimistPlayer", "OptimistPlayer", true)
{
	fVelocity = 2.5f;

	sPlayer.iKey = 1;
	sPlayer.displayName = "Optimist";
	sPlayer.iLevel = 1;
	sPlayer.iXP = 80;
	sPlayer.iAttackPower = 15;
	sPlayer.iDefensePower = 5;
	sPlayer.iGold = 10;
	sPlayer.iLife = 22;
	sPlayer.iLifeTotal = 22;
	sPlayer.iStamina = 5;
	sPlayer.iStaminaTotal = 5;
}
