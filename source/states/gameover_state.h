#ifndef _GAMEOVER_STATE_H
#define _GAMEOVER_STATE_H

#include "../defines.h"

class GameOverState : public StateMachineState
{
	public:
		GameOverState();
		virtual ~GameOverState();
		virtual void OnStart(void *);
		virtual void OnUpdate(f32);
		virtual void OnStop(void *);

	private:
		Milliseconds iBeginStateTime;
};

#endif // _GAMEOVER_STATE_H
