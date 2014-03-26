#include "guimanager.h"
#include "../scene/gamescene.h"
#include "../gameflow.h"
#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>

GuiManager *gGui = nullptr;

GuiManager::GuiManager()
	: pRocket(nullptr)
	, pContext(nullptr)
	, pDoc(nullptr)
	, pStackedDoc(nullptr)
{
	gGui = this;
}

GuiManager::~GuiManager()
{
	gGui = nullptr;
}

bool GuiManager::Initialize()
{
	pScreen->EnableCursor(true);
	return this->InitializeGUI();
}

bool GuiManager::Shutdown()
{
	RocketEventManager::RemoveListener(this);
	this->ReleaseGUI();
	return true;
}

bool GuiManager::UnloadStackedGUI()
{
	Log("Unloading Stacked GUI Document");
	if (pStackedDoc)
	{
		pStackedDoc->Hide();
		pContext->UnloadDocument(pStackedDoc);
		pStackedDoc->RemoveReference();
		pStackedDoc = nullptr;
	}

	return true;
}

bool GuiManager::LoadStackedGUI(const String &doc)
{
	if (this->UnloadStackedGUI())
	{
		Log("Loading Stacked GUI Document");
		pStackedDoc = pContext->LoadDocument(doc.c_str());
		if (pStackedDoc != nullptr)
		{
			auto title = pStackedDoc->GetElementById("title");
			if (title != nullptr)
				title->SetInnerRML(pStackedDoc->GetTitle());

			pStackedDoc->Focus();
			pStackedDoc->Show();
		}
	}

	return true;
}

bool GuiManager::UnloadGUI()
{
	Log("Unloading GUI Document");
	if (pDoc)
	{
		pDoc->Hide();
		pContext->UnloadDocument(pDoc);
		pDoc->RemoveReference();
		pDoc = nullptr;
	}

	return true;
}

bool GuiManager::ReloadGUI()
{
	return this->LoadGUI(sDocument);
}

bool GuiManager::LoadGUI(const String &doc)
{
	if (this->UnloadGUI())
	{
		Log("Loading GUI Document");
		pDoc = pContext->LoadDocument(doc.c_str());
		if (pDoc != nullptr)
		{
			auto title = pDoc->GetElementById("title");
			if (title != nullptr)
				title->SetInnerRML(pDoc->GetTitle());

			pDoc->Focus();
			pDoc->Show();

			if (pDoc->GetElementById("armor_aparence_div") != nullptr)
				pArmorPicture = pDoc->GetElementById("armor_aparence_div");

			if (pDoc->GetElementById("character_aparence_div") != nullptr)
				pAvatarPicture = pDoc->GetElementById("character_aparence_div");

			if (pDoc->GetElementById("ammo_aparence_div") != nullptr)
				pAmmoPicture = pDoc->GetElementById("ammo_aparence_div");

			if (pDoc->GetElementById("weapon_shotgun_div") != nullptr)
				pWeaponRifle = pDoc->GetElementById("weapon_shotgun_div");

			if (pDoc->GetElementById("weapon_super_shotgun_div") != nullptr)
				pWeaponShotgun = pDoc->GetElementById("weapon_super_shotgun_div");

			if (pDoc->GetElementById("weapon_nailgun_div") != nullptr)
				pWeaponNailgun = pDoc->GetElementById("weapon_nailgun_div");

			if (pDoc->GetElementById("weapon_super_nailgun_div") != nullptr)
				pWeaponHeavyNailgun = pDoc->GetElementById("weapon_super_nailgun_div");

			if (pDoc->GetElementById("weapon_rocketgun_div") != nullptr)
				pWeaponGrenadeLauchenr = pDoc->GetElementById("weapon_rocketgun_div");

			if (pDoc->GetElementById("weapon_super_rocketgun_div") != nullptr)
				pWeaponRocketLauncher = pDoc->GetElementById("weapon_super_rocketgun_div");

			if (pDoc->GetElementById("weapon_railgun_div") != nullptr)
				pWeaponShockgun = pDoc->GetElementById("weapon_railgun_div");

			if (pDoc->GetElementById("armor") != nullptr)
				pElementArmor = pDoc->GetElementById("armor");

			if (pDoc->GetElementById("life") != nullptr)
				pElementLife = pDoc->GetElementById("life");

			if (pDoc->GetElementById("weapon_ammo_shells") != nullptr)
				pElementAmmoShells = pDoc->GetElementById("weapon_ammo_shells");

			if (pDoc->GetElementById("weapon_ammo_nails") != nullptr)
				pElementAmmoNails = pDoc->GetElementById("weapon_ammo_nails");

			if (pDoc->GetElementById("weapon_ammo_rockets") != nullptr)
				pElementAmmoRockets = pDoc->GetElementById("weapon_ammo_rockets");

			if (pDoc->GetElementById("weapon_ammo_cells") != nullptr)
				pElementAmmoCells = pDoc->GetElementById("weapon_ammo_cells");

			if (pDoc->GetElementById("weapon_ammo_selected") != nullptr)
				pElementAmmoSelected = pDoc->GetElementById("weapon_ammo_selected");

			if (pDoc->GetElementById("sfx") != nullptr && gGameData->IsSfxEnabled())
				pDoc->GetElementById("sfx")->SetAttribute("checked", "");

			if (pDoc->GetElementById("bgm") != nullptr && gGameData->IsBgmEnabled())
				pDoc->GetElementById("bgm")->SetAttribute("checked", "");

			if (pDoc->GetElementById("fullscreen") != nullptr && gGameData->IsFullScreenEnabled())
				pDoc->GetElementById("fullscreen")->SetAttribute("checked", "");

		}

		sDocument = doc;
	}

	return true;
}

bool GuiManager::InitializeGUI()
{
	pRocket = sdNew(RocketInterface());
	pRocket->sName = "GUI";
	Rocket::Core::SetRenderInterface(pRocket);
	Rocket::Core::SetFileInterface(pRocket);
	Rocket::Core::SetSystemInterface(pRocket);
	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	pContext = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(pScreen->GetWidth(), pScreen->GetHeight()));
	if (pContext == nullptr)
	{
		Rocket::Core::Shutdown();
		return false;
	}

	Rocket::Core::String fonts[5];
	fonts[0] = "fonts/Delicious-Roman.otf";
	fonts[1] = "fonts/Delicious-Italic.otf";
	fonts[2] = "fonts/Delicious-Bold.otf";
	fonts[3] = "fonts/Delicious-BoldItalic.otf";
	fonts[4] = "fonts/dpquake_.ttf";

	for (u32 i = 0; i < sizeof(fonts) / sizeof(Rocket::Core::String); i++)
		Rocket::Core::FontDatabase::LoadFontFace(fonts[i]);

	Rocket::Debugger::Initialise(pContext);
	pRocket->SetCurrentContext(pContext);
	pInput->AddKeyboardListener(pRocket);
	pInput->AddPointerListener(pRocket);
	pRocket->SetZ(-1000.0f);

	RocketEventManager::AddListener(this);
	return true;
}

void GuiManager::ReleaseGUI()
{
	RocketEventManager::RemoveListener(this);

	pInput->RemovePointerListener(pRocket);
	pInput->RemoveKeyboardListener(pRocket);

	pContext->UnloadAllDocuments();

	if (pDoc)
	{
		pDoc->RemoveReference();
		pDoc = nullptr;
	}

	pContext->RemoveReference();

	Rocket::Core::Shutdown();
	sdDelete(pRocket);
}

void GuiManager::OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script)
{
	UNUSED(ev)
	Rocket::Core::StringList commands;
	Rocket::Core::StringUtilities::ExpandString(commands, script, ';');
	for (u32 i = 0; i < commands.size(); ++i)
	{
		Rocket::Core::StringList values;
		Rocket::Core::StringUtilities::ExpandString(values, commands[i], ' ');
		if (values.empty())
			return;

		if (values[0] == "goto" && values.size() > 1)
		{
			if (values[1] == "credits")
				gFlow->Credits();
			else if (values[1] == "menu")
				gFlow->Menu();
			else if (values[1] == "options")
				gFlow->Options();
			else if (values[1] == "game")
				gFlow->DoLoad("pathfind.scene");
		}
		else if (values[0] == "exit")
		{
			pSystem->Shutdown();
		}
		else if (values[0] == "toggle" && values.size() > 1)
		{
			if (values[1] == "sfx")
			{
				if (gGameData->IsSfxEnabled())
				{
					gGameData->SetSfxVolume(pSoundSystem->GetSfxVolume());
					pSoundSystem->SetSfxVolume(0.0f);
					gGameData->SetSfxEnabled(false);
				}
				else
				{
					pSoundSystem->SetSfxVolume(gGameData->GetSfxVolume());
					gGameData->SetSfxEnabled(true);
				}
			}
			else if (values[1] == "bgm")
			{
				if (gGameData->IsBgmEnabled())
				{
					gGameData->SetBgmVolume(pSoundSystem->GetMusicVolume());
					pSoundSystem->SetMusicVolume(0.0f);
					gGameData->SetBgmEnabled(false);
				}
				else
				{
					pSoundSystem->SetMusicVolume(gGameData->GetBgmVolume());
					gGameData->SetBgmEnabled(true);
				}
			}
			else if (values[1] == "fullscreen")
			{
				//this->ReleaseGUI();

				if (gGameData->IsFullScreenEnabled())
				{
					gGameData->SetFullScreenEnabled(false);
					pScreen->ToggleFullscreen();
					//pSystem->Shutdown();
				}
				else
				{
					gGameData->SetFullScreenEnabled(true);
					pScreen->ToggleFullscreen();
					//pSystem->Shutdown();
				}

				//this->InitializeGUI();
				//this->ReloadGUI();
			}
		}
	}
}

// GUI Elements

void GuiManager::OnGetArmor(const u32 armor)
{
	if (armor <= 0)			pArmorPicture->SetClassNames("armor_aparence_0");
	else if (armor == 100)	pArmorPicture->SetClassNames("armor_aparence_100");
	else if (armor == 150)	pArmorPicture->SetClassNames("armor_aparence_150");
	else if (armor == 200)	pArmorPicture->SetClassNames("armor_aparence_200");
}

void GuiManager::OnChangeWeapon(const u32 weapon, bool* weaponsGotten)
{
	switch (weapon)
	{
		case ItemTypes::Weapons::Rifle:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_on" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_shells");
		break;

		case ItemTypes::Weapons::Shotgun:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_on" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_shells");
		break;

		case ItemTypes::Weapons::Nailgun:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_on" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_nails");
		break;

		case ItemTypes::Weapons::HeavyNailgun:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_on" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_nails");
		break;

		case ItemTypes::Weapons::GrenadeLauncher:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_on" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_rockets");
		break;

		case ItemTypes::Weapons::RocketLauncher:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_on" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_rockets");
		break;

		case ItemTypes::Weapons::Shockgun:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_on" : "weapon_inactive");

			pAmmoPicture->SetClassNames("ammo_aparence_cells");
		break;

		default:
			pWeaponRifle->SetClassNames(weaponsGotten[ItemTypes::Weapons::Rifle] ? "weapon_shotgun_off" : "weapon_inactive");
			pWeaponShotgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shotgun] ? "weapon_super_shotgun_off" : "weapon_inactive");
			pWeaponNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Nailgun] ? "weapon_nailgun_off" : "weapon_inactive");
			pWeaponHeavyNailgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::HeavyNailgun] ? "weapon_super_nailgun_off" : "weapon_inactive");
			pWeaponGrenadeLauchenr->SetClassNames(weaponsGotten[ItemTypes::Weapons::GrenadeLauncher] ? "weapon_rocketgun_off" : "weapon_inactive");
			pWeaponRocketLauncher->SetClassNames(weaponsGotten[ItemTypes::Weapons::RocketLauncher] ? "weapon_super_rocketgun_off" : "weapon_inactive");
			pWeaponShockgun->SetClassNames(weaponsGotten[ItemTypes::Weapons::Shockgun] ? "weapon_railgun_off" : "weapon_inactive");
		break;
	}
}

void GuiManager::OnDamageAvatar(const u32 life)
{
	if (100 <= life && life > 80)		pAvatarPicture->SetClassNames("character_aparence_100");
	else if (80 <= life && life > 60)	pAvatarPicture->SetClassNames("character_aparence_80");
	else if (60 <= life && life > 40)	pAvatarPicture->SetClassNames("character_aparence_60");
	else if (40 <= life && life > 20)	pAvatarPicture->SetClassNames("character_aparence_40");
	else								pAvatarPicture->SetClassNames("character_aparence_20");
}

void GuiManager::SetArmor(u32 armor)
{
	if (armor > 0)
	{
		char x[100];
		snprintf(x, 100, "%d", armor);
		pElementArmor->SetInnerRML(Rocket::Core::String(x));
	}
	// Clean UI
	else
	{
		OnGetArmor(0);
		pElementArmor->SetInnerRML(Rocket::Core::String(""));
	}
}

void GuiManager::SetLife(u32 life)
{
	char x[100];
	snprintf(x, 100, "%d", life);
	pElementLife->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetAmmoShells(u32 quantity)
{
	char x[100];
	snprintf(x, 100, "%d", quantity);
	pElementAmmoShells->SetInnerRML(Rocket::Core::String(x));

	SetAmmoSelected(quantity);
}

void GuiManager::SetAmmoNails(u32 quantity)
{
	char x[100];
	snprintf(x, 100, "%d", quantity);
	pElementAmmoNails->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetAmmoRockets(u32 quantity)
{
	char x[100];
	snprintf(x, 100, "%d", quantity);
	pElementAmmoRockets->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetAmmoCells(u32 quantity)
{
	char x[100];
	snprintf(x, 100, "%d", quantity);
	pElementAmmoCells->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetAmmoSelected(u32 quantity)
{
	char x[100];
	snprintf(x, 100, "%d", quantity);
	pElementAmmoSelected->SetInnerRML(Rocket::Core::String(x));
}

ISceneObject *GuiManager::GetSceneObject() const
{
	return pRocket;
}
