// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#pragma once
#define WINDOW_TITLE "Bomberman - ICAD 2025"
#include "bomberman/rendering/debugRenderer.h"
#include "bomberman/entities/menu.h"
#include "bomberman/core/physics.h"
#include "bomberman/rendering/renderer.h"
//#define ENABLE_PIXELPERFECT

namespace bomberman
{
	static constexpr char passwords[11][21] = {
		"NMIHPPBPCAFHABDPCPCH",
		"HIJDIJFJDLHFLOPDJDJN",
		"BAJDINANMJGGCPOOLOLG",
		"DJOLBGLGKGJAHIEMNMNN",
		"NMKGDDONMHLCGKKGKGKJ",
		"ABGKKBPHILHFLOPCPCPC",	//"AAAAAAAAAAAAAAAAAAAA",
		"FEBABGLEFLHFLOPCPCPA",
		"HIFEMIIABJGGCPOBABAN",
		"NMEFPHCMNJGGCPOBABAF",
		"JDGKKBPHILHFLOPGKGKL",
		"HIPCOHCMNLHFLOPEFEFG"
	};
	enum  KillerType: int
	{
		FIRE = 0,
		ENEMY = 1,
		MISC = 2
	};

	enum GameState
	{
		MENU		= 0,
		LOADING		= 1,
		PLAY		= 2,
		GAMEOVER	= 3,
		ENDING		= 4,
		PASSWORD	= 5,
	};

	class Player;
}

namespace Tmpl8
{


class Game : public TheApp
{
public:
	static Game* GetInstance();

	~Game() override;
	void AddEntityToRenderQueue(bomberman::Entity* entity);

	void AddPhysicsBody(bomberman::PhysicsObject* body);
	void RemovePhysicsBody(uint id);

	bool IsKillerColliding(const float4& aabb, bomberman::KillerType killerType, bool autoKill=false);
	void NextLevel();

	void AddToScore(int amount);

	utils::vector<bomberman::Player*>& GetPlayers();
	void SetGameState(bomberman::GameState newState) { state = newState; }

	void Reset();

	// game flow methods
	void Init();
	void Tick( float deltaTime );
	void Shutdown();
	void Render();
	// input handling
	void MouseUp( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { mousePos.x = x, mousePos.y = y; }
	void MouseWheel( float ) { /* implement if you want to handle the mouse wheel */ }
	void KeyUp( int ) { /* implement if you want to handle keys */ }
	void KeyDown( int ) { /* implement if you want to handle keys */ }
	// data members
	int2 mousePos;

private:
	void MenuTick(float deltaTime);
	void LoadTick(float deltaTime);
	void PlayTick(float deltaTime);
	void GameOverTick(float deltaTime);
	void EndingTick(float deltaTime);
	void PasswordTick(float deltaTime);

	void RenderMenu();
	void RenderLoadScreen();
	void RenderPlayScreen();
	void RenderGameOverScreen();
	void RenderEnding();
	void RenderPassword();

	void InitializePlayers();



	bomberman::GameState state{ bomberman::GameState::MENU };
	bomberman::Renderer renderer{ {} };
	bomberman::DebugRenderer debugRenderer{};
	
	
	FrameBuffer screen1{};
	Shader* screenShader{ nullptr };

	utils::vector<bomberman::Player*> players{};
	utils::vector<bomberman::Entity*> entities{};
	//utils::vector<bomberman::Entity*> enemies{};


	utils::vector<FrameBuffer*> screens{};
	utils::vector<bomberman::Camera*> cameras{};
	bomberman::World world{ false, {0, 0} };

	int playerAmount = 2;

	bool restart = false;
	int level = 1;
	bool renderDebug = false;
	int score = 0;
	const int maxLives = 2;
	int lives = maxLives;
	Timer levelTimer{};
	int timeLeft = 200;

	/// -------------------- MENU items ------------------------///
	bomberman::Menu menu{};
	bomberman::Camera menuCamera{};


	//// Loading screen items -----///
	Timer loadingTimer{};


	//--- Password items ---//
	char password[21] = "AAAAAAAAAAAAAAAAAAAA";
	int currentPasswordIndex = 0;

	bomberman::Arrow arrow{};
};

} // namespace Tmpl8