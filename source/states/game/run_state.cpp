#include "run_state.h"
#include "../../manager/guimanager.h"
#include "../../scene/gamescene.h"

RunState::RunState()
{
}

RunState::~RunState()
{
}

void RunState::OnStart(void *data)
{
	Log("Entering Run State");

	if (data)
	{
		GameScene *g = static_cast<GameScene *>(data);
		g->Resume();
	}
	else
	{
		gGui->LoadGUI("gui/views/game.rml");
	}
}

void RunState::OnUpdate(f32 dt)
{
	UNUSED(dt)
}

void RunState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting Run State");
}
