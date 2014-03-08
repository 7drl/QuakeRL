#include "pause_state.h"
#include "../../manager/guimanager.h"
#include "../../scene/gamescene.h"

PauseState::PauseState()
{
}

PauseState::~PauseState()
{
}

void PauseState::OnStart(void *data)
{
	Log("Entering Pause State");
	gGui->LoadStackedGUI("gui/views/gamepause.rml");
	pSoundSystem->Mute();
	GameScene *g = static_cast<GameScene *>(data);
	g->Pause();
}

void PauseState::OnUpdate(f32 dt)
{
	UNUSED(dt)
}

void PauseState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting Pause State");
	gGui->UnloadStackedGUI();
	pSoundSystem->Unmute();
}
