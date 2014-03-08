#ifndef _MAIN_MENU_STATE_H
#define _MAIN_MENU_STATE_H

#include "../defines.h"

class MainMenuState : public StateMachineState
{
	public:
		MainMenuState();
		virtual ~MainMenuState();
		virtual void OnStart(void *);
		virtual void OnUpdate(f32);
		virtual void OnStop(void *);

	protected:
		Music	musTheme;
};

#endif // _MAIN_MENU_H
