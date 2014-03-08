#include "mainmenu_state.h"
#include "../manager/guimanager.h"
#include "../gameflow.h"

MainMenuState::MainMenuState()
{
}

MainMenuState::~MainMenuState()
{
	musTheme.Unload();
}

void MainMenuState::OnStart(void *data)
{
	pSoundSystem->Unmute();

	UNUSED(data)
	Log("Entering MainMenu State");

	gGui->LoadGUI("gui/views/mainmenu.rml");

	musTheme.Load("sounds/scottwills_time.ogg");
	musTheme.SetVolume(1.0f);

	if (gGameData && gGameData->IsBgmEnabled() == true)
		pSoundSystem->PlayMusic(&musTheme);

	gFlow->ResetCamera();
}

void MainMenuState::OnUpdate(f32 dt)
{
	UNUSED(dt)
}

void MainMenuState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting MainMenu State");

	pSoundSystem->StopMusic();
	musTheme.Unload();
}
