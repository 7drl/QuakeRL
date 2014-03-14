#ifndef _GUIMANAGER_H_
#define _GUIMANAGER_H_

#include "Defines.h"
#include <api/rocket/RocketInterface.h>
#include <Rocket/Core.h>

using namespace Seed;
using namespace Seed::RocketGui;

class GuiManager;
extern GuiManager *gGui;

class GuiManager : public IRocketEventListener
{
	SEED_DISABLE_COPY(GuiManager)
	public:
		GuiManager();
		virtual ~GuiManager();

		bool Initialize();
		bool Shutdown();

		ISceneObject *GetSceneObject() const;

		// GUI
		bool LoadGUI(const String &doc);
		bool ReloadGUI();
		bool UnloadGUI();
		void ReleaseGUI();

		bool LoadStackedGUI(const String &doc);
		bool UnloadStackedGUI();

		// IRocketEventListener
		virtual void OnGuiEvent(Rocket::Core::Event &ev, const Rocket::Core::String &script);

	private:
		bool InitializeGUI();
		void PrintHostage(u32 hostage);

	private:
		String				sDocument;

		// GUI
		RocketInterface			*pRocket;
		Rocket::Core::Context	*pContext;
		Rocket::Core::ElementDocument *pDoc;
		Rocket::Core::ElementDocument *pStackedDoc;

		// GUI Elements
		Rocket::Core::Element	*pElementSfx;
		Rocket::Core::Element	*pElementBgm;
};

#endif // _GUIMANAGER_H_
