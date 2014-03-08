#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../defines.h"

class GameScene;
class GameState : public StateMachineState
{
	public:
		GameState();
		virtual ~GameState();
		virtual void OnStart(void *);
		virtual void OnUpdate(f32);
		virtual void OnStop(void *);
		void LateStop();

		GameScene *pGame;
		bool bDoStop;
};

#endif // GAME_STATE_H
