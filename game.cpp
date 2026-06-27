// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "game.h"

#include <iostream>

#include "bomberman/core/audio.h"
#include "bomberman/core/levelManager.h"
#include "bomberman/entities/menu.h"
#include "bomberman/entities/player.h"


static Game* instance = nullptr;

namespace {
	//----- Global Variables ------// (can be shared across multiple Game class instances.
	
	//----- Functions ------//
	bomberman::Text textEntity{};
	const float2 baseTextSize = { 0.1f, -0.15f };
	const int2 letters[36]
	{
		{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
		{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1},
		{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2},
		{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3},
		{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4},
		{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5},
	};




	void RenderText(const char* text, const float2& position, float size, bomberman::Renderer& renderer)
	{

		textEntity.Initialize();
		textEntity.SetScale(baseTextSize * size);
		textEntity.SetPosition(position);
		float2 textPos = position;


		int textSize = static_cast<int>(strlen(text));

		for (int i = 0; i < textSize; i++)
		{
			char c = text[i];
			if (c == '\n') {
				textPos.x = position.x;
				textPos.y += textEntity.GetScale().y * 2.75f;
			}else if (c != ' ')
			{
				textEntity.SetPosition(textPos);
				if (IsCharAlpha(c))
				{
					textEntity.SetFrame(letters[c - 'a']);
				}
				else
				{
					textEntity.SetFrame(letters[26 + (c - '0')]);
				}
				renderer.RenderEntity(&textEntity);
			}

			textPos.x += textEntity.GetScale().x * 2;
		}
	}

}




utils::vector<bomberman::Player*>& Tmpl8::Game::GetPlayers()
{
	return players;
}

void Game::Reset()
{


	for (int i = 0; i < playerAmount; i++)
	{
		players[i]->Initialize();
	}
	bomberman::LevelManager::CleanUp();
	bomberman::LevelManager::LoadLevel(level, &world);
	renderer.clearBatch();
	bomberman::LevelManager::PrepareBatch(renderer);
	levelTimer.reset();
	entities.clear();
	if  (level != 6)
	{

		timeLeft = 200;
	} else
	{

		timeLeft = 30;
	}
}

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	instance = this;
	Audio::Initialize();
	bomberman::Assets::LoadAssets();

	menu.Initialize();
	arrow.Initialize();
	menuCamera.SetScreenSize({ 720, 720 });
	//textEntity.Initialize();


	screenShader = bomberman::Assets::GetShader(bomberman::ShaderType::SCREEN);
	bomberman::LevelManager::LoadLevel(level, &world);



}

// __ other game functions -- //
void Game::NextLevel()
{
	restart = true;
	level++;
	lives++;

	if (level > 11)
	{
		state = bomberman::ENDING;
	}
}

void Game::AddToScore(int amount)
{
	score += amount;
}

void Game::AddEntityToRenderQueue(bomberman::Entity* entity)
{
	entities.push_back(entity);
}

void Game::AddPhysicsBody(bomberman::PhysicsObject* body)
{
	world.AddRigidBody(body);
}

void Game::RemovePhysicsBody(uint id)
{
	world.RemoveBody(id);
}

bool Game::IsKillerColliding(const float4& aabb, bomberman::KillerType killerType, bool autoKill)
{
	for (bomberman::Player* player : players)
	{
		if (bomberman::PhysicsHandler::Collision(player->GetRigidbody()->GetAABB(), aabb))
		{
			if (autoKill) player->KillYourself(killerType);

			return true;
		}
	}

	return false;
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Game::Tick(float deltaTime)
{
	if (deltaTime > 0.2f) return;

	switch (state)
	{
	case bomberman::MENU:
		MenuTick(deltaTime);
		break;
	case bomberman::LOADING:
		LoadTick(deltaTime);
		break;
	case bomberman::PLAY:
		PlayTick(deltaTime);
		break;
	case bomberman::GAMEOVER:
		GameOverTick(deltaTime);
		break;
	case bomberman::ENDING:
		EndingTick(deltaTime);
		break;
	case bomberman::PASSWORD:
		PasswordTick(deltaTime);
		break;
	default:
		MenuTick(deltaTime);
		break;
	}
}

void Game::Render()
{
	switch (state)
	{
	case bomberman::MENU:
		RenderMenu();
		break;
	case bomberman::LOADING:
		RenderLoadScreen();
		break;
	case bomberman::PLAY:
		RenderPlayScreen();
		break;
	case bomberman::GAMEOVER:
		RenderGameOverScreen();
		break;
	case bomberman::ENDING:
		RenderEnding();
		break;
	case bomberman::PASSWORD:
		RenderPassword();
		break;
	default:
		RenderMenu();
		break;
	}
}


//--- Game States tick -- //
void Game::MenuTick(float deltaTime)
{
	Audio::PlaySound(SoundType::MENU_MUSIC, 0.5f);

	menuCamera.SetPosition({ 0, 0 });
	menu.Tick(deltaTime);

	if (GetAsyncKeyState(GLFW_KEY_P))
	{
		state = bomberman::GameState::PASSWORD;
	}

	if (GetAsyncKeyState(GLFW_KEY_1))
	{
		playerAmount = 1;
		state = bomberman::LOADING;

		InitializePlayers();


	}

	if (GetAsyncKeyState(GLFW_KEY_2))
	{
		playerAmount = 2;
		state = bomberman::LOADING;
		InitializePlayers();
	}
}

void Game::LoadTick(float deltaTime)
{
	Audio::StopSound(SoundType::MENU_MUSIC);
	Audio::PlaySound(SoundType::LOADING, 0.5f);

	constexpr float waitTime = 3.0f;

	if (loadingTimer.elapsed() > 4.0f) loadingTimer.reset();

	if (loadingTimer.elapsed() > waitTime)
	{
		loadingTimer.reset();
		state = bomberman::PLAY;
	}

	textEntity.Tick(deltaTime);

	if (level == 6)
	{
		for (bomberman::Player* player : players)
		{

			player->SetImmuneTo(bomberman::FIRE, true);
			player->SetImmuneTo(bomberman::ENEMY, true);
		}
	} else if (level > 6)
	{
		for (bomberman::Player* player : players)
		{

			player->SetImmuneTo(bomberman::FIRE, true);
			player->SetImmuneTo(bomberman::ENEMY, false);
		}
	}
}

void Game::PlayTick(float deltaTime)
{
	bool someoneDied = false;
	if (restart)
	{
		this->Reset();

		restart = false;
		return;
	}
	if (levelTimer.elapsed() > 1.0f)
	{
		levelTimer.reset();
		timeLeft--;
	}

	if (timeLeft <= 0)
	{
		timeLeft = 0;
		if (level == 6)
		{
			level++;
			this->Reset();
			loadingTimer.reset();
			state = bomberman::LOADING;
		} else
		{
			bomberman::LevelManager::TimeOver();
		}
	}

	Audio::PlaySound(SoundType::MAIN_MUSIC, 0.4f);
	world.Step(deltaTime);

	bomberman::LevelManager::UpdateMap(deltaTime);

	for (bomberman::Player* player : players)
	{
		player->Tick(deltaTime);

		if (player->GetState() == bomberman::DYING && player->GetScale().x == 0)
		{
			someoneDied = true;
		}

		if (bomberman::LevelManager::IsCollidingWithAnyEnemy(*player, *player->GetRigidbody(), false, true, &renderer))
		{
			player->KillYourself(bomberman::KillerType::ENEMY);
		}
	}

	if (someoneDied)
	{
		lives--;
		if (lives < 0)
		{
			state = bomberman::GAMEOVER;
			this->Reset();
		}
		else
		{

			state = bomberman::LOADING;
			loadingTimer.reset();
			this->Reset();
		}
	}

	//dev cheats
	if (GetAsyncKeyState(GLFW_KEY_3))
	{
		renderDebug = true;
	}
	if (GetAsyncKeyState(GLFW_KEY_4))
	{
		renderDebug = false;
	}

	if (GetAsyncKeyState(GLFW_KEY_H))
	{
		bomberman::LevelManager::Nuke();
	}
}

void Game::GameOverTick(float deltaTime)
{
	Audio::StopSound(SoundType::MENU_MUSIC);
	Audio::PlaySound(SoundType::LOADING, 0.5f);

	constexpr float waitTime = 3.0f;

	if (loadingTimer.elapsed() > 4.0f) loadingTimer.reset();

	if (loadingTimer.elapsed() > waitTime)
	{
		loadingTimer.reset();
		state = bomberman::MENU;
	}

	textEntity.Tick(deltaTime);
}

void Game::EndingTick(float)
{
	Audio::PlaySound(SoundType::MENU_MUSIC);
}

void Game::PasswordTick(float )
{
	static bool moveLastFrame = false;
	if (GetAsyncKeyState(GLFW_KEY_O))
	{
		state = bomberman::MENU;
	}
	//Spaghetti but i dont care :D
	if (GetAsyncKeyState(VK_RIGHT) && !moveLastFrame)
	{
		currentPasswordIndex++;
		if (currentPasswordIndex > 20) currentPasswordIndex = 20;
		moveLastFrame = true;
	}
	else if (GetAsyncKeyState(VK_LEFT) && !moveLastFrame)
	{

		currentPasswordIndex--;
		if (currentPasswordIndex <= 0) currentPasswordIndex = 0;
		moveLastFrame = true;
	}
	else if (GetAsyncKeyState(VK_UP) && !moveLastFrame)
	{
		password[currentPasswordIndex]++;
		if (password[currentPasswordIndex] < 'A') password[currentPasswordIndex] = 'A';
		moveLastFrame = true;
	}
	else if (GetAsyncKeyState(VK_DOWN) && !moveLastFrame)
	{
		password[currentPasswordIndex]--;
		if (password[currentPasswordIndex] > 'Z') password[currentPasswordIndex] = 'Z';
		moveLastFrame = true;
	}
	else if (!GetAsyncKeyState(VK_RIGHT) && !GetAsyncKeyState(VK_LEFT) &&
			!GetAsyncKeyState(VK_UP) && !GetAsyncKeyState(VK_DOWN) && 
		    !GetAsyncKeyState(GLFW_KEY_M)) {
		moveLastFrame = false;
	}


	if (GetAsyncKeyState(GLFW_KEY_M) && !moveLastFrame)
	{
		moveLastFrame = true;
		printf("Checking password\n");
		for (int i=0; i<11; i++)
		{
			bool correct = true;
			for (int j=0; j<21; j++)
			{
				if (password[j] != bomberman::passwords[i][j]) correct = false;
			}

			if (correct)
			{
				level = i + 1;
				this->Reset();
				SetGameState(bomberman::LOADING);
			}
		}
	}
}

///--- Game States Render ---//
void Game::RenderMenu()
{
	renderer.BindCamera(&menuCamera);
	renderer.RenderEntity(&menu);

	RenderText("press 1 or 2 to start", { -0.5f, -0.4 }, 0.3f, renderer);
}

void Game::RenderLoadScreen()
{

	if (level == 6) // 6==bonus
	{
		timeLeft = 30;
		RenderText("bonus", { -0.2f, 0 }, 0.3f, renderer);
	} else
	{

		string levelStr = "level " + std::to_string(level);
		RenderText(levelStr.c_str(), { -0.2f, 0 }, 0.3f, renderer);
	}
	renderer.BindCamera(&menuCamera);

}

void Game::RenderPlayScreen()
{
	if (restart) return;

	mat4 proj{};
	ortho(proj, 0, 1280, 720, 0, -1.0f, 1.0f);


	//Render the scene for each Screen/player
	for (int i = 0; i < players.length(); i++)
	{
		screens[i]->Bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//max - 630, -175
		// min
		cameras[i]->SetPosition(
			{
				//Camera position should be inverted.
				max(min(0.0f, -players[i]->GetPosition().x + (cameras[i]->GetScreenSize().x / 2)), -550.0f),
				0
			});
		cameras[i]->Update();

		renderer.BindCamera(cameras[i]);

		renderer.RenderBatch();

		bomberman::LevelManager::RenderBreakableTiles(renderer, cameras[i]);

		int entityCount = 0;
		for (bomberman::Entity* entity : entities)
		{
			// We only draw the entity if its actually inside of the view;
			bool isVisisble = -entity->GetPosition().x - entity->GetScale().x <= cameras[i]->GetPosition().x &&
				entity->GetPosition().x - entity->GetScale().x <= (cameras[i]->GetScreenSize().x - cameras[i]->GetPosition().x);
			if (isVisisble)
			{
				entityCount++;
				renderer.RenderEntity(entity);
			}
		}


		for (bomberman::Player* player : players)
		{
			renderer.RenderEntity(player);
		}


		if (renderDebug)
		{
			world.RenderBodies(debugRenderer);
			debugRenderer.Render(cameras[i]->GetMatrix());
			debugRenderer.ClearQueue();
		}


		renderer.BindCamera(nullptr);
		screens[i]->UnBind();

	}
	glClearColor(0, 0, 0, 1);
#ifdef ENABLE_PIXELPERFECT
	if (bomberman::collisionInfo.entityA != nullptr && bomberman::collisionInfo.entityB != nullptr)
	{
		
		static Shader depthShader{ "assets/shaders/animated.vert", "assets/shaders/depth.frag", false };
		FrameBuffer* framebuffer = new FrameBuffer{};
		framebuffer->Initialize({ 1280 / 2, 720 });
		framebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render the first entity
		{
			glClearColor(0, 0, 0, 1);

			bomberman::AnimatedEntity* entity = static_cast<bomberman::AnimatedEntity*>(bomberman::collisionInfo.entityA);
			mat4 matrix{};


			translate(matrix, entity->GetPosition());
			scale(matrix, entity->GetScale());

			Shader* shader = entity->GetShader();
			shader->Bind();
			entity->PrepareRender();

			depthShader.Bind();
			depthShader.SetFloat2("grid", entity->GetFrameSize());
			depthShader.SetFloat2("frame", entity->GetCurrentFrame());
			depthShader.SetFloat4("filter", entity->GetFilterColor());
			depthShader.SetInputTexture(1, "tex", entity->GetTexture());
			depthShader.SetInputMatrix("proj", proj);
			depthShader.SetFloat4("col", { 1, 0, 0, 1 });

			depthShader.SetInputMatrix("matrix", matrix);
			DrawQuad();
		}
		// Render the second entity
		{
			bomberman::AnimatedEntity* entity = static_cast<bomberman::AnimatedEntity*>(bomberman::collisionInfo.entityB);
			mat4 matrix{};


			translate(matrix, entity->GetPosition());
			scale(matrix, entity->GetScale());

			Shader* shader = entity->GetShader();
			shader->Bind();
			entity->PrepareRender();

			depthShader.Bind();

			depthShader.SetFloat2("grid", entity->GetFrameSize());
			depthShader.SetFloat2("frame", entity->GetCurrentFrame());
			depthShader.SetFloat4("filter", entity->GetFilterColor());
			depthShader.SetInputTexture(1, "tex", entity->GetTexture());
			depthShader.SetInputMatrix("proj", proj);
			depthShader.SetFloat4("col", { 1, 1, 0, 1 });

			depthShader.SetInputMatrix("matrix", matrix);
			DrawQuad();
		}

		framebuffer->UnBind();

		//glClear(GL_COLOR_BUFFER_BIT);
		//glClearColor(0, 0, 1, 1);
		//{
		//	mat4 matrix{};


		//	translate(matrix, { (1280 / playerAmount) * (1 + 0.5f), 720 * 0.5f });
		//	scale(matrix, { (1280 / playerAmount) / 2, 720.0f * 0.4f });

		//	screenShader->Bind();
		//	screenShader->SetInputMatrix("matrix", matrix);
		//	screenShader->SetInputMatrix("proj", proj);
		//	glActiveTexture(GL_TEXTURE2);
		//	glBindTexture(GL_TEXTURE_2D, framebuffer->GetID());
		//	screenShader->SetInt("screen", 2);
		//	screenShader->SetFloat4("color", { 1, 1, 0, 1 });
		//}

		//DrawQuad();

		// lets read through the buffer
		bool isCollision = false;

		int bufSize = (1280 / 2) * 720;
		float* pixels = new float[bufSize];
		framebuffer->Bind();
		glReadPixels(0, 0, 1280 / 2, 720, GL_RED, GL_FLOAT, pixels);
		delete framebuffer;

		for (int p=0; p<bufSize;p++)
		{
			if (pixels[p] >= 0.3f)
			{
				isCollision = true;
				break;
			}
		}
		delete[] pixels;
		if (isCollision)
		{
			static_cast<bomberman::Player*>(bomberman::collisionInfo.entityA)->KillYourself(bomberman::KillerType::ENEMY);
		}


		bomberman::collisionInfo.entityA = nullptr;
		bomberman::collisionInfo.entityB = nullptr;

	}
#endif
	//Render each screen to a quad on the final screen.
	for (int i = 0; i < playerAmount; i++)
	{
		mat4 matrix{};

		translate(matrix, { (1280 / playerAmount) * (i + 0.5f), 720 * 0.5f });
		scale(matrix, { (1280.0f / playerAmount) / 2, 720.0f * 0.4f });

		screenShader->Bind();
		screenShader->SetInputMatrix("matrix", matrix);
		screenShader->SetInputMatrix("proj", proj);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, screens[i]->GetID());
		screenShader->SetInt("screen", 2);
		screenShader->SetFloat4("color", { 1, 1, 0, 1 });
		DrawQuad();

	}



	renderer.BindCamera(&menuCamera);

	RenderText(std::string("score " + std::to_string(score)).c_str(), {-0.9f, 0.85f}, 0.3f, renderer);

	string livesText = "lives " + std::to_string(lives);
	RenderText(livesText.c_str(), { 0.0f, 0.85f }, 0.3f, renderer);


RenderText((std::string("time ") + std::to_string(timeLeft)).c_str(), { 0.55f, 0.85f }, 0.3f, renderer);
	

	entities.clear();
}

void Game::RenderGameOverScreen()
{
	renderer.BindCamera(&menuCamera);
	RenderText("game over", { -0.2f, 0 }, 0.3f, renderer);

}

void Game::RenderEnding()
{

	renderer.BindCamera(&menuCamera);
	for (int i=0; i<players.length(); i++)
	{
		players[i]->SetDirection(bomberman::DOWN);

		players[i]->SetPosition({ -0.4f + i * 0.25f, -0.9f });
		players[i]->SetScale({ 0.1f, -0.1f });

		renderer.RenderEntity(players[i]);
	}

	RenderText("congratulations\n\nyou have succeeded in helping bomberman\nto become a human being\nmaybe you can recognize him\nin another hudson soft game\n\n good bye", { -0.87f, 0.85f }, 0.2f, renderer);

}

void Game::RenderPassword()
{

	renderer.BindCamera(&menuCamera);

	char lowercasePassword[21];
	for (int i=0; i<21; i++)
	{
		lowercasePassword[i] = password[i] + 32;
	}

	RenderText("type in password", float2{ -0.75f, 0.35f }, 0.4f, renderer);
	RenderText(lowercasePassword, float2{ -0.75f, 0.0f }, 0.4f, renderer);

	arrow.SetPosition({ -0.75f + (0.075f * currentPasswordIndex), -0.2f });
	renderer.RenderEntity(&arrow);
}

void Game::InitializePlayers()
{

	for (int i = 0; i < playerAmount; i++)
	{
		bomberman::Player* newPlayer = new bomberman::Player{};
		newPlayer->Initialize();

		players.push_back(newPlayer);

		//---
		FrameBuffer* newScreen = new FrameBuffer{};
		newScreen->Initialize({ 1280, 720 });
		screens.push_back(newScreen);

		//--
		bomberman::Camera* newCamera = new bomberman::Camera{};
		newCamera->SetScreenSize({ 1280 / (2.0f * playerAmount), 720 / 1.7 });
		cameras.push_back(newCamera);

	}

	for (int i = 0; i < playerAmount; i++)
	{
		world.AddRigidBody(players[i]->GetRigidbody());
	}

	bomberman::LevelManager::PrepareBatch(renderer);


}

///-----------

Game* Game::GetInstance()
{
	return instance;
}

Game::~Game()
{
}

void Game::Shutdown()
{
	entities.clear();

	for (bomberman::Player* player : players)
	{
		delete player;
	}
	players.clear();

	for (FrameBuffer* framebuffer : screens)
	{
		delete framebuffer;
	}
	screens.clear();


	for (bomberman::Camera* camera : cameras)
	{
		delete camera;
	}
	cameras.clear();
	Audio::CleanUp();
	renderer.clearBatch();

	bomberman::LevelManager::CleanUp();
	bomberman::Assets::UnloadAssets();
}
