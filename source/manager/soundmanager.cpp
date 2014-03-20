#include "soundmanager.h"

SoundManager::SoundManager()
	: pSounds(nullptr)
{
}

void SoundManager::Init(SceneNode &sounds)
{
	pSounds = &sounds;
}

void SoundManager::Play(const String &name)
{
	auto source = static_cast<ISoundSource *>(pSounds->GetChildByName(name));
	if (source == nullptr)
		return;

	source->Play();
}

void SoundManager::Update(f32 dt)
{
	UNUSED(dt)
}
