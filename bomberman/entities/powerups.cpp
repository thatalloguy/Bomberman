#include "precomp.h"
#include "powerups.h"

#include "bomb.h"
#include "game.h"
#include "../core/levelManager.h"
#include "player.h"



void bomberman::PowerUp::Explode()
{
	BreakableTile::Explode();

	if (type == BREAKABLE)
	{
		type = POWERUP;
		index = this->GetPowerUpSprite();
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		LevelManager::SpawnEnemy(this->mapIndex);
	}
}

void bomberman::PowerUp::Tick(float deltaTime)
{
	if (type == EMPTY) return;

	BreakableTile::Tick(deltaTime);

	if (Game::GetInstance()->IsKillerColliding({ this->position.x + 4, this->position.y + 4, this->position.x + 8, this->position.y + 8 }, KillerType::MISC))
	{
		Activate();
		type = EMPTY;
		index = { 11, 4 };
	}
}



void bomberman::SpeedPowerUp::Activate()
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		player->SetMovementSpeed(player->GetMovementSpeed() * 1.5f);
	}
}

void bomberman::BombPowerUp::Activate()
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		player->SetMaxBombs(player->GetMaxBombs() + 2);
		if (player->GetMaxBombs() > 5)
		{
			player->SetMaxBombs(5);
		}
	}
}

void bomberman::BombWalkPowerUp::Activate()
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		player->SetCanWalkThroughTile(TileType::BOMB, true);
	}
}

void bomberman::WallWalkPowerUp::Activate()
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		player->SetCanWalkThroughTile(TileType::BREAKABLE, true);
	}
}

void bomberman::MysteryPowerUp::Activate()
{
	active = true;
	timer.reset();

	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		printf("MysteryPowerUp::Tick! %i\n", player->GetPlayerID());
		player->SetImmuneTo(FIRE, true);
		player->SetImmuneTo(ENEMY, true);
	}
}

void bomberman::MysteryPowerUp::Tick(float deltaTime)
{
	PowerUp::Tick(deltaTime);

	if (!active) return;

	if (timer.elapsed() > 35)
	{
		for (Player* player : Game::GetInstance()->GetPlayers())
		{
			player->SetImmuneTo(FIRE, false);
			player->SetImmuneTo(ENEMY, false);
		}

		active = false;
	}

}

void bomberman::DetonatorPowerUp::Activate()
{
	isActive = true;

	for (Player* player : Game::GetInstance()->GetPlayers())
	{

		printf("DetonatorPowerUp::Tick! %i\n", player->GetPlayerID());
		player->SetBombExplodeOnSpawn(false);
	}
}

void bomberman::DetonatorPowerUp::Tick(float deltaTime)
{
	PowerUp::Tick(deltaTime);

	if (!isActive) return;

	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		if (GetAsyncKeyState(controllers[player->GetPlayerID()][5]))
		{
			for (int i=0; i<5; i++)
			{
				if (Tile* tile = player->GetPlacedOnTiles()[i])
				{
					tile->GetBomb()->Activate();
				}

			}
		}
	}
	
}

void bomberman::FlameProofPowerUp::Activate()
{
	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		player->SetImmuneTo(KillerType::FIRE, true);
	}
}

void bomberman::FlamePowerUp::Activate()
{
	Bomb::explosionRange++;
}
