#include "precomp.h"
#include "bonuses.h"

#include "../core/events.h"
#include "game.h"
#include "../core/levelManager.h"
#include "player.h"

namespace Global
{
	bool isAnyTileBroken = false;
	bool isAnyEnemyDead = false;
	int bombChained = 0;
	int exitBombedCount = 0;

	bomberman::Event OnTileBroken(const bomberman::EventData& )
	{
		isAnyTileBroken = true;
	}
	bomberman::Event OnEnemyKilled(const bomberman::EventData& )
	{
		printf("A enemy died\n");
		isAnyEnemyDead = true;
	}

	bomberman::Event OnBombChain(const bomberman::EventData& )
	{
		bombChained++;
		printf("chain %i\n", bombChained);
	}

	bomberman::Event OnExitChain(const bomberman::EventData& )
	{
		exitBombedCount++;
	}
}


void bomberman::Bonus::Initialize()
{
	BreakableTile::Initialize();
	Global::isAnyTileBroken = false;
	Global::isAnyEnemyDead = false;
	Global::bombChained = 0;
	Global::exitBombedCount = 0;
}

void bomberman::Bonus::Tick(float deltaTime)
{
	Tile::Tick(deltaTime);

	switch (state)
	{
	case INACTIVE:
		InactiveTick(deltaTime);
		break;
	case ACTIVE:
		ActiveTick(deltaTime);
		break;
	case COMPLETED:
		CompletedTick(deltaTime);
		break;
	default:
		break;
	}
}

//---- B panel bonus
void bomberman::BpanelBonus::Initialize()
{
	Bonus::Initialize();
	Events::AddListener(ENEMY_KILLED, Global::OnEnemyKilled);
}

void bomberman::BpanelBonus::Tick(float deltaTime)
{
	if (Global::isAnyEnemyDead)
	{
		state = FAILED;
	}

	Bonus::Tick(deltaTime);
}

void bomberman::BpanelBonus::InactiveTick(float )
{
	Exit* exit = LevelManager::GetExit();
	if (Game::GetInstance()->IsKillerColliding({ exit->GetPosition().x + 4, exit->GetPosition().y + 4, exit->GetPosition().x + 8, exit->GetPosition().y + 8 }, MISC))
	{
		state = ACTIVE;
		printf("bonus now Completed");

		index = { 8, 14 };
	}
}

void bomberman::BpanelBonus::ActiveTick(float )
{
	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(10*1000);
		state = COMPLETED;
		printf("Completed");
	}
}

void bomberman::BpanelBonus::CompletedTick(float )
{

}

//---- Goddess bonus

void bomberman::GoddessBonus::Initialize()
{
	Bonus::Initialize();
}

void bomberman::GoddessBonus::Tick(float deltaTime)
{
	Bonus::Tick(deltaTime);
}

void bomberman::GoddessBonus::InactiveTick(float )
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		int2 playerIndex = { static_cast<int>(player->GetPosition().x / 32), static_cast<int>(player->GetPosition().y / 32) };
		if (playerIndex.x == 1)
		{
			//west
			rowsCompleted[player->GetPlayerID()][3] = true;
		}
		else if (playerIndex.x == 25 )
		{
			//east
			rowsCompleted[player->GetPlayerID()][1] = true;
		}
		else if (playerIndex.y == 1 )
		{
			//north
			rowsCompleted[player->GetPlayerID()][0] = true;
		}
		else if (playerIndex.y == 11 )
		{
			//south
			rowsCompleted[player->GetPlayerID()][2] = true;
		}
		else
		{
			for (int i = 0; i < 4; i++) rowsCompleted[player->GetPlayerID()][i] = false;
		}


		if (rowsCompleted[player->GetPlayerID()][0] &&
			rowsCompleted[player->GetPlayerID()][1] &&
			rowsCompleted[player->GetPlayerID()][2] &&
			rowsCompleted[player->GetPlayerID()][3])
		{
			state = ACTIVE;
			index = { 9, 14 };
		}
	}
}

void bomberman::GoddessBonus::ActiveTick(float )
{

	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(20*1000);
		state = COMPLETED;
		printf("Completed");
	}
	
}

void bomberman::GoddessBonus::CompletedTick(float )
{
}

//---- Cola bonus

void bomberman::ColaBonus::Initialize()
{
	Bonus::Initialize();
	Events::AddListener(ENEMY_KILLED, Global::OnEnemyKilled);
}

void bomberman::ColaBonus::Tick(float deltaTime)
{
	if (Global::isAnyEnemyDead)
	{
		state = FAILED;
		printf("Failed bonus\n");
	}

	Bonus::Tick(deltaTime);
}

void bomberman::ColaBonus::InactiveTick(float )
{
	Exit* exit = LevelManager::GetExit();
	if (Game::GetInstance()->IsKillerColliding({ exit->GetPosition().x + 4, exit->GetPosition().y + 4, exit->GetPosition().x + 8, exit->GetPosition().y + 8 }, MISC))
	{
		timer.reset();
		state = ACTIVE;
		printf("bonus now Completed");
	}
}

void bomberman::ColaBonus::ActiveTick(float )
{
	if (!GetAsyncKeyState(GLFW_KEY_A))
	{
		printf("failed\n");
		state = INACTIVE;
	}

	if (timer.elapsed() > 15)
	{
		printf("WIN!\n");


		index = { 10, 14 };
		state = COMPLETED;

	}
}

void bomberman::ColaBonus::CompletedTick(float )
{
	if (GetIndex().x == 6) return;

	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(30 * 1000);
		state = COMPLETED;
		printf("Completed");
	}
}
//// ----- Famicom

void bomberman::FamicomBonus::Initialize()
{
	Bonus::Initialize();
	Events::AddListener(BOMB_CHAINED, Global::OnBombChain);
}

void bomberman::FamicomBonus::Tick(float deltaTime)
{
	Bonus::Tick(deltaTime);
}

void bomberman::FamicomBonus::InactiveTick(float )
{
	if (GetAsyncKeyState(GLFW_KEY_B))
	{
		Global::bombChained = 247;
	}
	if (Global::bombChained >= 248)
	{
		index = { 11, 14 };
		state = ACTIVE;
		
	}
}

void bomberman::FamicomBonus::ActiveTick(float )
{
	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(500 * 1000);
		state = COMPLETED;
		printf("Completed");
	}
}

void bomberman::FamicomBonus::CompletedTick(float )
{
}

///--- Nakamoto


void bomberman::NakamotoBonus::Initialize()
{
	Bonus::Initialize();
	Events::AddListener(TILE_BROKEN, Global::OnTileBroken);
}


void bomberman::NakamotoBonus::InactiveTick(float )
{
	if (Global::isAnyTileBroken)
	{
		printf("Failed\n");
		state = FAILED;
	}

	if (!LevelManager::AnyEnemiesAlive())
	{
		index = { 12, 14 };
		state = ACTIVE;
	}
}

void bomberman::NakamotoBonus::ActiveTick(float )
{
	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(10 * 1000000);
		state = COMPLETED;
		printf("Completed");
	}
}

void bomberman::NakamotoBonus::CompletedTick(float )
{
}
///---- Dezeniman Bonus

void bomberman::DezenimanBonus::Initialize()
{
	Bonus::Initialize();
	timeCheck.reset();
	Events::AddListener(EXIT_BOMBED, Global::OnExitChain);
}

void bomberman::DezenimanBonus::InactiveTick(float )
{
	if (!LevelManager::AnyEnemiesAlive())
	{
		printf("Failed\n");
		state = FAILED;
	}
	if (!LevelManager::AnyBreakableTilesPresent() && Global::exitBombedCount >= 3)
	{
		state = ACTIVE;
		index = { 13, 14 };
	}

}

void bomberman::DezenimanBonus::ActiveTick(float )
{
	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		this->SetType(EMPTY);
		this->SetIndex({ 6, 0 });
		Game::GetInstance()->AddToScore(20 * 1000000);
		state = COMPLETED;
		printf("Completed");
	}
}

void bomberman::DezenimanBonus::CompletedTick(float )
{
}
