#ifndef _GAMEFLOW_H_
#define _GAMEFLOW_H_

#include "Defines.h"
#include "states/mainmenu_state.h"
#include "states/options_state.h"
#include "states/credits_state.h"
#include "states/game_state.h"
#include "data/gamedata.h"

using namespace Seed;

class GameFlow;
extern GameFlow *gFlow;
extern GameData *gGameData;

class GameFlow : public IGameApp,
			public IEventSystemListener,
			public IEventInputKeyboardListener
{
	SEED_DISABLE_COPY(GameFlow)

	public:
		GameFlow();
		virtual ~GameFlow();

		inline Camera *GetCamera() const;
		inline SceneNode *GetScene() const;

		void LoadSceneFile(const String &file);
		const String &GetSceneFile() const;

		void ResetCamera();

		void DoLoad(const String &scene = "");
		void Menu();
		void Credits();
		void Options();

		virtual bool Initialize();
		virtual bool Update(f32 dt);
		virtual bool Shutdown();

		// IEventSystemListener
		virtual void OnSystemShutdown(const EventSystem *ev) override;

		// IEventInputKeyboardListener
		virtual bool OnInputKeyboardRelease(const EventInputKeyboard *ev) override;

	private:
		bool SaveSystemFlow() const;

	private:
		SceneNode			*pScene;
		Camera				*pCamera;
		Presentation		cPres;
		String				sDocument;

		// State Machine
		StateMachine		cFlow;

		// State Machine states
		MainMenuState		cMenu;
		OptionsState		cOptions;
		CreditsState		cCredits;
		GameState			cGame;
		StateMachineState	cLoad;

		// State Machine Events
		StateMachineEvent	cOnMenu;
		StateMachineEvent	cOnOptions;
		StateMachineEvent	cOnCredits;
		StateMachineEvent	cOnGame;
		StateMachineEvent	cOnLoad;

		// State Machine transitions
		StateMachineTransition cMenuToGame;
		StateMachineTransition cMenuToOptions;
		StateMachineTransition cMenuToCredits;
		StateMachineTransition cOptionsToMenu;
		StateMachineTransition cCreditsToMenu;
		StateMachineTransition cGameToMenu;
		StateMachineTransition cGameToLoad;
		StateMachineTransition cLoadToGame;

		// GUI Elements
		Rocket::Core::Element	*pElementLife;
		Rocket::Core::Element	*pElementTime;
		Rocket::Core::Element	*pElementHostage;
		Rocket::Core::Element	*pElementSfx;
		Rocket::Core::Element	*pElementBgm;

		String sSceneFile;

		Vector3f vCameraPos;
};

inline Camera *GameFlow::GetCamera() const
{
	return pCamera;
}

inline SceneNode *GameFlow::GetScene() const
{
	return pScene;
}

#endif // _GAMEFLOW_H_
