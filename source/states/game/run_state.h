#ifndef _RUN_STATE_H
#define _RUN_STATE_H

#include "../../defines.h"

class RunState : public StateMachineState
{
	public:
		RunState();
		virtual ~RunState();
		virtual void OnStart(void *);
		virtual void OnUpdate(f32);
		virtual void OnStop(void *);
};

#endif // _RUN_STATE_H
