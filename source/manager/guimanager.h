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

		void OnDamageAvatar(const u32 life);
		void OnGetArmor(const u32 armor);
		void OnChangeWeapon(const u32 weapon, bool *weaponsGotten);

		void SetArmor(u32 armor);
		void SetLife(u32 life);
		void SetAmmoShells(u32 quantity);
		void SetAmmoNails(u32 quantity);
		void SetAmmoRockets(u32 quantity);
		void SetAmmoCells(u32 quantity);
		void SetAmmoSelected(u32 quantity);

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
		Rocket::Core::Element	*pArmorPicture;
		Rocket::Core::Element	*pAvatarPicture;
		Rocket::Core::Element	*pAmmoPicture;

		// Weapons
		Rocket::Core::Element	*pWeaponShotgun;
		Rocket::Core::Element	*pWeaponRifle;
		Rocket::Core::Element	*pWeaponNailgun;
		Rocket::Core::Element	*pWeaponHeavyNailgun;
		Rocket::Core::Element	*pWeaponGrenadeLauchenr;
		Rocket::Core::Element	*pWeaponRocketLauncher;
		Rocket::Core::Element	*pWeaponShockgun;

		Rocket::Core::Element	*pElementArmor;
		Rocket::Core::Element	*pElementLife;
		Rocket::Core::Element	*pElementAmmoShells;
		Rocket::Core::Element	*pElementAmmoNails;
		Rocket::Core::Element	*pElementAmmoRockets;
		Rocket::Core::Element	*pElementAmmoCells;
		Rocket::Core::Element	*pElementAmmoSelected;

		Rocket::Core::Element	*pElementSfx;
		Rocket::Core::Element	*pElementBgm;
};

#endif // _GUIMANAGER_H_
