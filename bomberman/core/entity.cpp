#include "precomp.h"
#include "entity.h"
#include "../entities/bomb.h"
#include "../entities/enemy.h"
#include "events.h"
#include "game.h"
#include "levelManager.h"
#include "../entities/player.h"

bomberman::Tile::~Tile()
{
	delete bomb;
}

void bomberman::Tile::Tick(float deltaTime)
{
	if (bomb != nullptr)
	{
		if (bomb->GetState() == BombState::EXPLODED)
		{
			delete bomb;
			bomb = nullptr;
		} else
		{
			bomb->Tick(deltaTime);
		}
	}
}

void bomberman::Tile::SpawnBomb(const float2& spawnPos, bool explode)
{
	printf("Spawning bomb");
	bomb = new Bomb{};
	bomb->Initialize();
	bomb->SetPosition(spawnPos);//(this->position - (this->scale / 2));
	bomb->SetParent(this);

	if (explode) bomb->Activate();
}

void bomberman::Tile::RemoveBomb()
{
	delete bomb;
	bomb = nullptr;
}

void bomberman::Tile::Explode()
{
	Events::CallEvent(EventType::TILE_BROKEN, {});
	if (bomb != nullptr)
	{
		bomb->Explode();
		Events::CallEvent(EventType::BOMB_CHAINED, {});
	}
}


void bomberman::BreakableTile::Tick(float deltaTime)
{

	Tile::Tick(deltaTime);

	if (state > 0 && timer.elapsed() > 0.2 && type == BREAKABLE)
	{
		state++;
		index.x = 4.0f + state;
		timer.reset();
	}


	if (state == 6 && type == BREAKABLE)
	{
		type = EMPTY;
		index = { 6, 0 };
	}
}

void bomberman::Exit::Explode()
{
	if (type == BREAKABLE)
	{
		type = EXIT;
		index = {11, 3};
		return;
	}


	Events::CallEvent(EventType::EXIT_BOMBED, {});
	for (int i = 0; i < 2; i++)
	{
		LevelManager::SpawnEnemy(this->mapIndex, new Valcom{});
	}

}

void bomberman::Exit::Tick(float deltaTime)
{
	BreakableTile::Tick(deltaTime);

	if (state == 6)
	{
		type = EXIT;
		index = {11, 3};
	}

	if (!LevelManager::AnyEnemiesAlive() && Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, MISC))
	{
		Game::GetInstance()->SetGameState(LOADING);
		Game::GetInstance()->NextLevel();
	}
}

