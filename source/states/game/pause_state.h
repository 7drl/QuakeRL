#ifndef _PAUSE_STATE_H
#define _PAUSE_STATE_H

#include "../../defines.h"

class PauseState : public StateMachineState
{
	public:
		PauseState();
		virtual ~PauseState();
		virtual void OnStart(void *);
		virtual void OnUpdate(f32);
		virtual void OnStop(void *);
};

#endif // _PAUSE_STATE_H
