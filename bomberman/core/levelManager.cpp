#include "precomp.h"
#include "levelManager.h"

#include <fstream>
#include <iostream>

#include "../entities/bonuses.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "game.h"
#include "../entities/powerups.h"

namespace {

	int2 currentTileIndex = { 0, 0 };
	bomberman::Tile* map[MAP_WIDTH][MAP_HEIGHT];
	utils::vector<bomberman::Enemy*> enemies;
	utils::vector<bomberman::EnemyType> enemyBucket;
	utils::vector<bomberman::Tile*> breakableTiles{};

	bomberman::Exit* exitTile = nullptr;


	//Credits to: https://web.archive.org/web/20181018225423/http://tasvideos.org/RandomGenerators.html#Bomberman
	//Also I used Claud to help converting the assembly to c(++).
	//For the original Bomberman random function.
	unsigned char GetRand()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
		static int seed[3] = { rand() % 10,rand() % 10,rand() % 10 };
		static bool carry = false;

		int tempVal = carry << 8;

		tempVal = ((seed[0] << 4) | ((tempVal & 0x100) >> 5) | (seed[0] >> 5)) ^ 0x87;
		tempVal = (tempVal & 0xFF) + ((tempVal & 0x100) >> 8) + seed[1];
		seed[0] = tempVal;

		tempVal = ((seed[0] << 4) | ((tempVal & 0x100) >> 5) | (seed[0] >> 5)) ^ 0x48;
		seed[0] = tempVal;

		tempVal = (tempVal & 0xFF) + ((tempVal & 0x100) >> 8) + seed[2];
		seed[1] = tempVal;

		tempVal = (tempVal & 0xFF) + ((tempVal & 0x100) >> 8) + seed[0];

		if (!++seed[2])
		{
			seed[2] += 29;
		}

		return static_cast<unsigned char>(tempVal ^ seed[2]);
	}

	bool hasSwappedEnemies = false;
	bool isBonusLevel = false;
	bool hasNukedMap = false;
}


void bomberman::LevelManager::CreateNewTile(const int2& tileSprite, TileType type)
{
	Tile* tile = type == BREAKABLE ? new BreakableTile : new Tile;


	tile->SetIndex(tileSprite);
	tile->SetMapIndex({ currentTileIndex.x, currentTileIndex.y });
	tile->SetType(type);
	tile->SetScale(bomberman::tileSize);
	tile->SetPosition({
		tileInitialOffset.x + currentTileIndex.x * tileSize.x,
		tileInitialOffset.y + currentTileIndex.y * tileSize.y,
	});

	tile->Initialize();

	// So if the tile isnt batched the position is a bit weird so we have to correct it.
	if (type == BREAKABLE)
	{
		tile->SetPosition({
				tileInitialOffset.x * 0.2f + currentTileIndex.x * tileSize.x,
				tileInitialOffset.y * 0.6f + currentTileIndex.y * tileSize.y,
			});
		tile->SetScale(tileSize / 2);
		breakableTiles.push_back(tile);
	}

	map[currentTileIndex.x][currentTileIndex.y] = tile;

	currentTileIndex.x++;

}

void bomberman::LevelManager::UpdateMap(float deltaTime)
{

	if (GetAsyncKeyState(GLFW_KEY_K))
	{
		for (Enemy* enemy : enemies)
		{
			enemy->KillYourselfNow();
		}
	}
	for (int i = 0; i < MAP_WIDTH; i++)
	{
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			if (map[i][j]->HasBomb())
			{
				map[i][j]->Tick(deltaTime);
			}
		}
	}
	for (int i=0; i<breakableTiles.length(); i++)
	{
		breakableTiles[i]->Tick(deltaTime);
	}
	int aliveEnemyCount = 0;
	for (Enemy* enemy : enemies)
	{
		if (!enemy->IsDead()) aliveEnemyCount++;
		enemy->Tick(deltaTime);
	}

	if (isBonusLevel && aliveEnemyCount < 6)
	{
		SpawnEnemy({ 5, 5 }, new Valcom{});
	}
}

void bomberman::LevelManager::SpawnSpecialTiles(int level)
{
	if (breakableTiles.length() <= 0) return;


	int randomTile = rand() % breakableTiles.length();
	{
		BreakableTile* randTile = static_cast<BreakableTile*>(breakableTiles[randomTile]);

		BreakableTile* exit = new Exit{};
		exit->Initialize(); 
#ifdef DEBUG_MAP
		exit->SetIndex({ 14, 3 });
#else
		exit->SetIndex({ 4, 3 });
#endif
		exit->SetPosition(randTile->GetPosition());
		exit->SetScale(tileSize / 2);
		exit->SetType(BREAKABLE);
		exit->SetMapIndex(randTile->GetMapIndex());



		int2 mapIndex = randTile->GetMapIndex();


		delete map[mapIndex.x][mapIndex.y];
		breakableTiles[randomTile] = exit;
		exitTile = static_cast<Exit*>(exit);
		map[mapIndex.x][mapIndex.y] = exit;
	}

	int randomPowerupTile = rand() % breakableTiles.length();
	{
		while (randomTile == randomPowerupTile) randomPowerupTile = rand() % breakableTiles.length();

		BreakableTile* randTile = static_cast<BreakableTile*>(breakableTiles[randomPowerupTile]);
		PowerUp* powerup = nullptr;
		switch (level)
		{
		case 1:
			powerup = new BombWalkPowerUp{};//FlamePowerUp{};
			break;
		case 2:
			powerup = new BombPowerUp{};
			break;
		case 3:
			powerup = new DetonatorPowerUp{};
			break;
		case 4:
			powerup = new SpeedPowerUp{};
			break;
		case 5:
			powerup = new FlameProofPowerUp{};
			break;
		case 6:
			break;
		case 7:
			powerup = new MysteryPowerUp{};
			break;
		case 8:
			powerup = new FlamePowerUp{};
			break;
		case 9:
			powerup = new BombWalkPowerUp{};
			break;
		case 10:
			powerup = new MysteryPowerUp{};
			break;
		case 11:
			powerup = new BombPowerUp{};
			break;
		default:
			break;
		}

		if (powerup) {
			//level == 1 ? static_cast<PowerUp*>(new SpeedPowerUp) : new BombPowerUp;
			powerup->Initialize();
			powerup->SetIndex({ 0, 15 });
			powerup->SetPosition(randTile->GetPosition());
			powerup->SetScale(tileSize / 2);
#ifdef DEBUG_MAP
			powerup->SetIndex({ 14, 3 });
#else
			powerup->SetIndex({ 4, 3 });
#endif
			powerup->SetMapIndex(randTile->GetMapIndex());



			int2 mapIndex = randTile->GetMapIndex();


			delete map[mapIndex.x][mapIndex.y];
			breakableTiles[randomPowerupTile] = powerup;
			map[mapIndex.x][mapIndex.y] = powerup;
		}
	}

	int randomBonusTile = rand() % breakableTiles.length();
	{
		while (randomPowerupTile == randomBonusTile) randomBonusTile = rand() % breakableTiles.length();

		BreakableTile* randTile = static_cast<BreakableTile*>(breakableTiles[randomBonusTile]);

		Bonus* bonus = nullptr;
		switch (level)
		{
		case 1:
			bonus = new GoddessBonus{};
			break;
		case 2:
			bonus = new NakamotoBonus{};
			break;
		case 3:
			bonus = new FamicomBonus{};
			break;
		case 4:
			bonus = new ColaBonus{};
			break;
		case 5:
			bonus = new DezenimanBonus{};
			break;
		case 6:
			bonus = nullptr;
			break;
		case 7:
			bonus = new BpanelBonus{};
			break;
		case 8:
			bonus = new GoddessBonus{};
			break;
		case 9:
			bonus = new BpanelBonus{};
			break;
		case 10:
			bonus = new GoddessBonus{};
			break;
		case  11:
			bonus = new NakamotoBonus{};
			break;
		default:
			bonus = new DezenimanBonus{};
			break;
		}
		if (bonus) {
			bonus->Initialize();
			//bonus->SetIndex({ 0, 15 });
			bonus->SetPosition(randTile->GetPosition());
			bonus->SetScale(tileSize / 2);
			bonus->SetType(EMPTY);
#ifdef DEBUG_MAP
			bonus->SetIndex({ 14, 3 });
#else
			bonus->SetIndex({ 6, 0 });
#endif
			bonus->SetMapIndex(randTile->GetMapIndex());



			int2 mapIndex = randTile->GetMapIndex();


			delete map[mapIndex.x][mapIndex.y];
			breakableTiles[randomBonusTile] = bonus;
			map[mapIndex.x][mapIndex.y] = bonus;
		}
	}


	


}

void bomberman::LevelManager::RenderBreakableTiles(Renderer& renderer, Camera* camera)
{
	renderer.BindCamera(camera);
	for (Tile* tile : breakableTiles)
	{
		bool isVisible = -tile->GetPosition().x - tile->GetScale().x <= camera->GetPosition().x &&
			tile->GetPosition().x - tile->GetScale().x <= (camera->GetScreenSize().x - camera->GetPosition().x);
		if (isVisible) renderer.RenderEntity(tile);
	}
}

void bomberman::LevelManager::SpawnEnemy(const int2& index, Enemy* instance)
{
	Enemy* enemy =  instance;
	if (!enemy) {
		switch (enemyBucket.back())
		{
		case  EnemyType::VALCOM:
			enemy = new Valcom{};
			break;
		case EnemyType::ONEAL:
			enemy = new Oneal{};
			break;
		case EnemyType::DALL:
			enemy = new Dall{};
			break;
		case EnemyType::MINVO:
			enemy = new Minvo{};
			break;
		case EnemyType::OVAPI:
			enemy = new Ovapi{};
			break;
		case EnemyType::KONDORIA:
			enemy = new Kondoria{};
			break;
		case EnemyType::PARSE:
			enemy = new Parse{};
			break;
		case EnemyType::PONTAN:
			enemy = new Pontan{};
			break;
		case EnemyType::UNKNOWN:
			enemy = new DebugEnemy{};
			break;
		default:
			enemy = new Valcom{};
		}

	}

	enemy->SetPosition({
		tileInitialOffset.x * 0.2f + index.x * tileSize.x,
		tileInitialOffset.y * 0.6f + index.y * tileSize.y,
		});
	enemy->Initialize();

	enemies.push_back(enemy);

	enemyBucket.pop_back();
}

bool bomberman::LevelManager::CanSpawnEnemy()
{
	return enemyBucket.length() > 0;
}

void bomberman::LevelManager::Nuke()
{
	if (hasNukedMap) return;
	hasNukedMap = true;

	for (int i = 0; i < MAP_WIDTH; i++)
	{
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			map[i][j]->Explode();
		}
	}
}

void bomberman::LevelManager::TimeOver()
{
	if (hasSwappedEnemies) return;
	hasSwappedEnemies = true;

	printf("Swapping\n");
	for (int i=0; i<enemies.length(); i++)
	{
		if (!enemies[i]->IsDead())
		{
			Enemy* newEnemy = nullptr;
			if (rand() % 10 == 1)
			{
				newEnemy = new DebugEnemy{};
			} else
			{
				newEnemy = new Pontan{};
			}

			newEnemy->SetPosition(enemies[i]->GetPosition());
			newEnemy->Initialize();
			delete enemies[i];
			enemies[i] = newEnemy;
		}
	}
}

bool bomberman::LevelManager::AnyBreakableTilesPresent()
{
	for (Tile* tile : breakableTiles)
	{
		if (tile->GetType() == BREAKABLE)
		{
			return true;
		}
	}

	return false;
}


bool bomberman::LevelManager::AnyEnemiesAlive()
{
	for (Enemy* enemy : enemies)
	{
		if (!enemy->IsDead())
		{
			return true;
		}
	}
	return false;
}

bool bomberman::LevelManager::IsCollidingWithAnyEnemy(Entity& entity,const PhysicsObject& object, bool autoKill, bool pixelPerfect, Renderer* renderer)
{
	for (Enemy* enemy : enemies)
	{

		if (PhysicsHandler::Collision(object.GetAABB(), enemy->GetBody().GetAABB()))
		{
			if (autoKill) enemy->KillYourselfNow();

			if (pixelPerfect && renderer)
			{
				renderer->SetupPixelPerfectCollision(entity, *enemy, object.GetAABB(), enemy->GetBody().GetAABB());
			}

			return true;
		}
	}

	return false;
}

bomberman::Tile& bomberman::LevelManager::GetTile(const int2& index)
{
	if (index.x < 0 || index.y < 0) return *map[0][0];
	else if (index.x >= MAP_WIDTH || index.y >= MAP_HEIGHT) return *map[MAP_WIDTH-1][MAP_HEIGHT-1];
	return *map[index.x][index.y];
}

bomberman::Exit* bomberman::LevelManager::GetExit()
{
	return exitTile;
}


bool bomberman::LevelManager::LoadLevel(int levelId, bomberman::World*)
{
	hasSwappedEnemies = false;
	hasNukedMap = false;
	currentTileIndex = { 0, 0 };
	enemyBucket.clear();
	isBonusLevel = false;
	printf("level %i-----\n", levelId);
#ifndef CHASE_MAP
	switch (levelId)
	{
	case 1:
		for (int i = 0; i < 6; i++)
		{
			enemyBucket.push_back(EnemyType::VALCOM);
		}

		break;
	case 2:
		for (int i = 0; i < 3; i++)
		{
			enemyBucket.push_back(EnemyType::VALCOM);
		}
		for (int i = 0; i < 3; i++)
		{
			enemyBucket.push_back(EnemyType::ONEAL);
		}
		break;
	case 3:
		enemyBucket.push_back(EnemyType::VALCOM);
		enemyBucket.push_back(EnemyType::VALCOM);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		break;
	case 4:
		enemyBucket.push_back(EnemyType::VALCOM);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		break;
	case 5:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		break;
	case 6:
		printf("BONUS LEVEL!\n");
		isBonusLevel = true;
		for (int i = 0; i < 6; i++) enemyBucket.push_back(EnemyType::VALCOM);
		break;
	case 7:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		break;
	case 8:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::OVAPI);
		enemyBucket.push_back(EnemyType::OVAPI);
		break;
	case 9:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		break;
	case 10:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::OVAPI);
		enemyBucket.push_back(EnemyType::KONDORIA);
		break;
	case 11:
		enemyBucket.push_back(EnemyType::ONEAL);
		enemyBucket.push_back(EnemyType::DALL);
		enemyBucket.push_back(EnemyType::MINVO);
		enemyBucket.push_back(EnemyType::OVAPI);
		enemyBucket.push_back(EnemyType::KONDORIA);
		enemyBucket.push_back(EnemyType::KONDORIA);
		enemyBucket.push_back(EnemyType::KONDORIA);
		break;
	default:
		enemyBucket.push_back(EnemyType::UNKNOWN);
		enemyBucket.push_back(EnemyType::UNKNOWN);
		enemyBucket.push_back(EnemyType::UNKNOWN);
		break;
	}
#else
	for (int i = 1; i < 9; i++) enemyBucket.push_back(static_cast<EnemyType>(i));
#endif

	string line{};
	ifstream file("levels/level1.csv");
	printf("Loading level\n");
	if (file.is_open())
	{
		while (!file.eof()) {
			getline(file, line);
			for (int i = 0; i < line.length(); i++)
			{
				if (line[i] != ',')
				{
					switch (stoi(line.substr(i, 2)))
					{
					case 7:
						CreateNewTile({ 7, 0 });
						break;
					case 75:
						CreateNewTile({ 3, 3 }, SOLID);
						break;

					case 96:
						CreateNewTile({ 3, 4 });
						break;
					case 6:
						if (GetRand() % 20 > 18 && CanSpawnEnemy() && i > 2)
						{
							SpawnEnemy(currentTileIndex);
							CreateNewTile({ 7, 0 });
						}
						else if (GetRand() % 20 > 13 && !isBonusLevel)
						{
							CreateNewTile({ 4, 3 }, BREAKABLE);
						}
						else
						{
							CreateNewTile({ 7, 0 });
						}
						break;
					default:
						CreateNewTile({ 13, 0 }, UNKNOWN);
						break;
					}
					i++;

				}

			}

			currentTileIndex.y++;
			currentTileIndex.x = 0;
		}
		file.close();
	}
	else
	{
		FatalError("Could not Load level: levels/level%i.csv\n", levelId);
		return false;
	}
	if (!isBonusLevel) {
		SpawnSpecialTiles(levelId);
	}

	printf("loaded level\n");
	return true;
}

//unused
void bomberman::LevelManager::CleanUp()
{
	breakableTiles.clear();
	for (int i = 0; i < MAP_WIDTH; i++)
	{
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			delete map[i][j];
			map[i][j] = nullptr;
		}
	}

	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

void bomberman::LevelManager::PrepareBatch(Renderer& renderer)
{
	for (int i=0; i<MAP_WIDTH; i++)
	{
		for (int j=0; j<MAP_HEIGHT; j++)
		{
			if (map[i][j] == nullptr) return;
			renderer.AddTileToBatch(map[i][j]);
		}
	}

	renderer.PrepareBatch();
}

