#include "options_state.h"
#include "../manager/guimanager.h"

OptionsState::OptionsState()
{
}

OptionsState::~OptionsState()
{
}

void OptionsState::OnStart(void *data)
{
	UNUSED(data)
	Log("Entering Options State");

	gGui->LoadGUI("gui/views/options.rml");
}

void OptionsState::OnUpdate(f32 dt)
{
	UNUSED(dt)
}

void OptionsState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting Options State");
}
