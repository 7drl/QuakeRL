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
			Rocket::Core::Element *title = pStackedDoc->GetElementById("title");
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
			Rocket::Core::Element *title = pDoc->GetElementById("title");
			if (title != nullptr)
				title->SetInnerRML(pDoc->GetTitle());

			pDoc->Focus();
			pDoc->Show();

			if (pDoc->GetElementById("playerName") != nullptr)
				pElementPlayerName = pDoc->GetElementById("playerName");

			if (pDoc->GetElementById("displayEnemyName") != nullptr)
				pElementEnemyName = pDoc->GetElementById("displayEnemyName");

			if (pDoc->GetElementById("level") != nullptr)
				pElementLevel = pDoc->GetElementById("level");

			if (pDoc->GetElementById("xp") != nullptr)
				pElementXP = pDoc->GetElementById("xp");

			if (pDoc->GetElementById("attackPower") != nullptr)
				pElementAttackPower = pDoc->GetElementById("attackPower");

			if (pDoc->GetElementById("gold") != nullptr)
				pElementGold = pDoc->GetElementById("gold");

			if (pDoc->GetElementById("life") != nullptr)
				pElementLife = pDoc->GetElementById("life");

			if (pDoc->GetElementById("stamina") != nullptr)
				pElementStamina = pDoc->GetElementById("stamina");

			if (pDoc->GetElementById("enemy_div") != nullptr)
				pEnemyPicture = pDoc->GetElementById("enemy_div");

			if (pDoc->GetElementById("levelxp_enemy") != nullptr)
				pElementLevelXPBox = pDoc->GetElementById("levelxp_enemy");

			if (pDoc->GetElementById("enemy_level") != nullptr)
				pElementEnemyLevel = pDoc->GetElementById("enemy_level");

			if (pDoc->GetElementById("enemy_life") != nullptr)
				pElementEnemyLife = pDoc->GetElementById("enemy_life");

			if (pDoc->GetElementById("dialog_title") != nullptr)
				pElementDialogTitle = pDoc->GetElementById("dialog_title");

			if (pDoc->GetElementById("dialog") != nullptr)
				pDialog = pDoc->GetElementById("dialog");

			if (pDoc->GetElementById("avatar_a") != nullptr && pDoc->GetElementById("avatar_b") != nullptr && pDoc->GetElementById("avatar_c") != nullptr)
			{
				pHeroPicture[0] = pDoc->GetElementById("avatar_a");
				pHeroPicture[1] = pDoc->GetElementById("avatar_b");
				pHeroPicture[2] = pDoc->GetElementById("avatar_c");
			}

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

	Rocket::Core::String fonts[4];
	fonts[0] = "fonts/Delicious-Roman.otf";
	fonts[1] = "fonts/Delicious-Italic.otf";
	fonts[2] = "fonts/Delicious-Bold.otf";
	fonts[3] = "fonts/Delicious-BoldItalic.otf";

	for (unsigned int i = 0; i < sizeof(fonts) / sizeof(Rocket::Core::String); i++)
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
	for (size_t i = 0; i < commands.size(); ++i)
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
void GuiManager::SetPlayerName(const String &name)
{
	pElementPlayerName->SetInnerRML(Rocket::Core::String(name.c_str()));
}

void GuiManager::SetEnemyName(const String &name)
{
	pElementEnemyName->SetInnerRML(Rocket::Core::String(name.c_str()));
}

void GuiManager::SetDialog(const String &text)
{
	pDialog->SetInnerRML(Rocket::Core::String(text.c_str()));
}

void GuiManager::SelectHero(const String &name)
{
	if (name == "Optimist")
	{
		pHeroPicture[0]->SetClassNames("avatar_pessimist_off");
		pHeroPicture[1]->SetClassNames("avatar_optimist_on");
		pHeroPicture[2]->SetClassNames("avatar_realist_off");
	}
	if (name == "Realist")
	{
		pHeroPicture[0]->SetClassNames("avatar_optimist_off");
		pHeroPicture[1]->SetClassNames("avatar_realist_on");
		pHeroPicture[2]->SetClassNames("avatar_pessimist_off");
	}
	if (name == "Pessimist")
	{
		pHeroPicture[0]->SetClassNames("avatar_realist_off");
		pHeroPicture[1]->SetClassNames("avatar_pessimist_on");
		pHeroPicture[2]->SetClassNames("avatar_optimist_off");
	}
}

void GuiManager::SelectEnemy(const String &hero, u32 enemyId)
{
	if (hero == "Optimist") {
		switch (enemyId)
		{
			case 0: pEnemyPicture->SetClassNames("enemy_1_optimist"); break;
			case 1: pEnemyPicture->SetClassNames("enemy_2_optimist"); break;
			case 2: pEnemyPicture->SetClassNames("enemy_3_optimist"); break;
			case 3: pEnemyPicture->SetClassNames("enemy_4_optimist"); break;
			default: break;
		}

		pElementEnemyName->SetClassNames("");
		pDialog->SetClassNames("");
		pElementLevelXPBox->SetClassNames("");
		pElementDialogTitle->SetClassNames("");
	}
	else if (hero == "Realist")
	{
		switch (enemyId)
		{
			case 0: pEnemyPicture->SetClassNames("enemy_1_realist"); break;
			case 1: pEnemyPicture->SetClassNames("enemy_2_realist"); break;
			case 2: pEnemyPicture->SetClassNames("enemy_3_realist"); break;
			case 3: pEnemyPicture->SetClassNames("enemy_4_realist"); break;
			default: break;
		}

		pElementEnemyName->SetClassNames("");
		pDialog->SetClassNames("");
		pElementLevelXPBox->SetClassNames("");
		pElementDialogTitle->SetClassNames("");
	}
	else if (hero == "Pessimist")
	{
		switch (enemyId)
		{
			case 0: pEnemyPicture->SetClassNames("enemy_1_pessimist"); break;
			case 1: pEnemyPicture->SetClassNames("enemy_2_pessimist"); break;
			case 2: pEnemyPicture->SetClassNames("enemy_3_pessimist"); break;
			case 3: pEnemyPicture->SetClassNames("enemy_4_pessimist"); break;
			default: break;
		}

		pElementEnemyName->SetClassNames("");
		pDialog->SetClassNames("");
		pElementLevelXPBox->SetClassNames("");
		pElementDialogTitle->SetClassNames("");
	}
	else
	{
		pEnemyPicture->SetClassNames("");
		pElementEnemyName->SetClassNames("hide");
		pDialog->SetClassNames("hide");
		pElementLevelXPBox->SetClassNames("hide");
		pElementDialogTitle->SetClassNames("hide");
	}
}

void GuiManager::SetLevel(u32 level)
{
	char x[100];
	snprintf(x, 100, "%d", level);
	pElementLevel->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetEnemyLevel(u32 level)
{
	char x[100];
	snprintf(x, 100, "%d", level);
	pElementEnemyLevel->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetXP(u32 xp)
{
	char x[100];
	snprintf(x, 100, "%d", xp);
	pElementXP->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetAttackPower(u32 attackPower)
{
	char x[100];
	snprintf(x, 100, "%d", attackPower);
	pElementAttackPower->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetGold(u32 gold)
{
	char x[100];
	snprintf(x, 100, "%d", gold);
	pElementGold->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetLife(u32 life, u32 lifeTotal)
{
	char x[100];
	snprintf(x, 100, "%d / %d", life, lifeTotal);
	pElementLife->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetEnemyLife(u32 life, u32 lifeTotal)
{
	char x[100];
	snprintf(x, 100, "%d / %d", life, lifeTotal);
	pElementEnemyLife->SetInnerRML(Rocket::Core::String(x));
}

void GuiManager::SetStamina(u32 stamina, u32 staminaTotal)
{
	char x[100];
	snprintf(x, 100, "%d / %d", stamina, staminaTotal);
	pElementStamina->SetInnerRML(Rocket::Core::String(x));
}

ISceneObject *GuiManager::GetSceneObject() const
{
	return pRocket;
}
