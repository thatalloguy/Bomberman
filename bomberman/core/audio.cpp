#include "precomp.h"
#include "audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "lib/miniaudio.h"

namespace 
{
	ma_engine* engine{nullptr};

	ma_sound sounds[Audio::numSounds];
}

void Audio::Initialize()
{
	engine = new ma_engine();
	if (ma_engine_init(NULL, engine) != MA_SUCCESS)
	{
		printf("HELP HELP HELP HELP AUDIO HELP!\n");
		delete engine;
		return;
	}

	for (int i=0; i<Audio::numSounds; i++)
	{
		ma_sound_init_from_file(engine, _soundFiles[i], 0, NULL, NULL, &sounds[i]);
		printf("loaded sound: %s \n", _soundFiles[i]);
	}
}

void Audio::CleanUp()
{
	for (int i = 0; i < Audio::numSounds; i++)
	{
		ma_sound_uninit(&sounds[i]);
		printf("unloaded sound: %s \n", _soundFiles[i]);
	}

	ma_engine_uninit(engine);
	delete engine;
}

void Audio::PlaySound(SoundType type, float volume)
{
	if (ma_sound_is_playing(&sounds[static_cast<int>(type)])) return;

	ma_sound_set_volume(&sounds[static_cast<int>(type)], volume);
	ma_sound_start(&sounds[static_cast<int>(type)]);
}

void Audio::StopSound(SoundType type)
{
	if (!ma_sound_is_playing(&sounds[static_cast<int>(type)])) return;

	ma_sound_stop(&sounds[static_cast<int>(type)]);
}
