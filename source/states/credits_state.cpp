#include "credits_state.h"
#include "../manager/guimanager.h"

CreditsState::CreditsState()
{
}

CreditsState::~CreditsState()
{
}

void CreditsState::OnStart(void *data)
{
	UNUSED(data)
	Log("Entering Credits State");

	gGui->LoadGUI("gui/views/credits.rml");
}

void CreditsState::OnUpdate(f32 dt)
{
	UNUSED(dt)
}

void CreditsState::OnStop(void *data)
{
	UNUSED(data)
	Log("Exiting Credits State");
}
