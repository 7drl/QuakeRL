#ifndef _GUIMANAGER_H_
#define _GUIMANAGER_H_

#include "Defines.h"
#include <api/rocket/RocketInterface.h>
#include <Rocket/Core.h>

using namespace Seed;
using namespace Seed::RocketGui;

class GuiManager;
extern GuiManager *gGui;

class GuiManager : public IRocketEventListener
{
	SEED_DISABLE_COPY(GuiManager)
	public:
		GuiManager();
		virtual ~GuiManager();

		bool Initialize();
		bool Shutdown();

		ISceneObject *GetSceneObject() const;

		// GUI
		bool LoadGUI(const String &doc);
		bool ReloadGUI();
		bool UnloadGUI();
		void ReleaseGUI();

		bool LoadStackedGUI(const String &doc);
		bool UnloadStackedGUI();

		void SetPlayerName(const String &name);
		void SetEnemyName(const String &name);
		void SelectHero(const String &name);
		void SelectEnemy(const String &hero = "", u32 enemyId = 0);
		void SetDialog(const String &text);
		void SetLevel(u32 level);
		void SetEnemyLevel(u32 level);
		void SetXP(u32 xp);
		void SetAttackPower(u32 attackPower);
		void SetGold(u32 gold);
		void SetLife(u32 life, u32 lifeTotal);
		void SetEnemyLife(u32 life, u32 lifeTotal);
		void SetStamina(u32 stamina, u32 staminaTotal);

		// IRocketEventListener
		virtual void OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script);

	private:
		bool InitializeGUI();
		void PrintHostage(u32 hostage);

	private:
		String				sDocument;

		// GUI
		RocketInterface			*pRocket;
		Rocket::Core::Context	*pContext;
		Rocket::Core::ElementDocument *pDoc;
		Rocket::Core::ElementDocument *pStackedDoc;

		// GUI Elements
		Rocket::Core::Element	*pElementEnemyName;
		Rocket::Core::Element	*pElementLevelXPBox;
		Rocket::Core::Element	*pElementPlayerName;
		Rocket::Core::Element	*pElementDialogTitle;
		Rocket::Core::Element	*pElementLevel;
		Rocket::Core::Element	*pElementXP;
		Rocket::Core::Element	*pElementAttackPower;
		Rocket::Core::Element	*pElementGold;
		Rocket::Core::Element	*pElementLife;
		Rocket::Core::Element	*pElementStamina;
		Rocket::Core::Element	*pElementStaminaPotion;
		Rocket::Core::Element	*pElementEnemyLevel;
		Rocket::Core::Element	*pElementEnemyLife;

		Rocket::Core::Element	*pHeroPicture[3];
		Rocket::Core::Element	*pEnemyPicture;
		Rocket::Core::Element	*pDialog;

		Rocket::Core::Element	*pElementSfx;
		Rocket::Core::Element	*pElementBgm;
};

#endif // _GUIMANAGER_H_
