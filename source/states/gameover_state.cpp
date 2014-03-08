#include "gameover_state.h"
#include "../gameflow.h"
#include "../manager/guimanager.h"
#include "../scene/gamescene.h"

GameOverState::GameOverState()
	: iBeginStateTime(0)
{
}

GameOverState::~GameOverState()
{
}

void GameOverState::OnStart(void *data)
{
	Log("Entering GameOver State");
	gGui->LoadGUI("gui/views/gameover.rml");
	pSoundSystem->Mute();

	iBeginStateTime = pTimer->GetMilliseconds();

	GameScene *g = static_cast<GameScene *>(data);
	g->Pause();
}

void GameOverState::OnUpdate(f32 dt)
{
	UNUSED(dt);
	if (iBeginStateTime + 3000 < pTimer->GetMilliseconds())
		gFlow->Menu();
}

void GameOverState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting GamePause State");
	pSoundSystem->Unmute();
}

