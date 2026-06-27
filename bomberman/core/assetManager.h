#pragma once

#ifndef TEXTURE_AMOUNT
#define TEXTURE_AMOUNT 4
#endif


#ifndef SHADER_AMOUNT
#define SHADER_AMOUNT 6
#endif

namespace bomberman
{
	struct ShaderConfig
	{
		const char* vertPath;
		const char* fragPath;
	};

	enum class TextureType
	{
		TILES	 = 0,
		ARROW	 = 1,
		MENU	 = 2,
		TEXT	 = 3,
	};

	enum class ShaderType
	{
		DEFAULT		= 0,
		ANIMATED	= 1,
		DEBUG		= 2,
		DEPTH		= 3,
		SCREEN		= 4,
		TILE		= 5
	};


	static constexpr const char* texturePaths[4] = {
		"assets/tiles.png",
		"assets/arrow.png",
		"assets/mainscreen.png",
		"assets/text.png"
	};

	static constexpr ShaderConfig shaderPaths[6] = {
		ShaderConfig{"assets/shaders/default.vert", "assets/shaders/default.frag"},
		ShaderConfig{"assets/shaders/animated.vert", "assets/shaders/animated.frag"},
		ShaderConfig{"assets/shaders/debug.vert", "assets/shaders/debug.frag"},
		ShaderConfig{"assets/shaders/default.vert", "assets/shaders/depth.frag"},
		ShaderConfig{"assets/shaders/screen.vert", "assets/shaders/screen.frag"},
		ShaderConfig{"assets/shaders/tile.vert", "assets/shaders/tile.frag"},
	};


	namespace Assets
	{
		bool LoadAssets();
		void UnloadAssets();

		GLTexture* GetTexture(TextureType type);
		Shader* GetShader(ShaderType type);
	}
;
}