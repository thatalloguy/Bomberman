#pragma once

enum class SoundType
{
	MENU_MUSIC = 0,
	MAIN_MUSIC = 1,
	WALKING_LR = 2,
	PLACE_BOMB = 3,
	EXPLOSION = 4,
	LOADING = 5,
	WALKING_UD = 6,
};

namespace Audio
{
	constexpr int numSounds = 7;
	static const char* _soundFiles[numSounds] = {
		"assets/audio/mainmenu.wav",
		"assets/audio/stage1.wav",
		"assets/audio/step_LR.wav",
		"assets/audio/bombPlace.wav",
		"assets/audio/explosion.wav",
		"assets/audio/loading.wav",
		"assets/audio/step_UD.wav",
	};

	void Initialize();
	void CleanUp();

	void PlaySound(SoundType type, float volume=1.0f);
	void StopSound(SoundType type);
}