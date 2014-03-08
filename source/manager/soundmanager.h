#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

#include "../defines.h"

class SoundManager
{
	public:
		SoundManager();

		void Init(SceneNode &sounds);
		void Play(const String &name);
		void Update(f32 dt);

	private:
		SceneNode *pSounds;
};

#endif
