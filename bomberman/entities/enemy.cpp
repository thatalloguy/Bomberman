#include "precomp.h"
#include "enemy.h"

#include "../core/events.h"
#include "game.h"
#include "../core/levelManager.h"
#include "player.h"

inline bool HasLineOfSight(const int2&a, const int2& b)
{
	// so we have C here to see on which axis we should check.
	int2 c = a - b;


	int length = c.x + c.y;

	// this looks weird but basically
	// if the length is < 0 we than do a i-- for loop
	// otherwise we do a normal i++ loop
	for (int i=0; length< 0 ? i>length : i<length; length < 0 ? i-- : i++ )
	{
		// So we only wanna check for the axis that isnt 0.
		bomberman::TileType type = bomberman::LevelManager::GetTile(int2{ 
			c.x == 0 ? b.x : b.x + i, c.y == 0 ? b.y : b.y + i
		}).GetType();
		if (type != bomberman::EMPTY) return false;
	}

	return true;
}

//Returns the direction need to player
// If the enemy cant see that player it will return -1;
static int GetDirectionToPlayer(const bomberman::Enemy& enemy, const bomberman::Player& player)
{
	const int2 playerGridPos = int2{static_cast<int>(floor(player.GetPosition().x / 32)) , static_cast<int>(floor(player.GetPosition().y / 32)) };
	const int2 enemyGridPos = int2{ static_cast<int>(floor(enemy.GetPosition().x / 32)), static_cast<int>(floor(enemy.GetPosition().y / 32))};


	if (playerGridPos.x - 1 == enemyGridPos.x || playerGridPos.y == enemyGridPos.y) {
		if (HasLineOfSight(playerGridPos, enemyGridPos))
		{
			if (playerGridPos.y == enemyGridPos.y)
			{
				if (playerGridPos.x > enemyGridPos.x)
				{
					return bomberman::LEFT;
				}

				if (playerGridPos.x < enemyGridPos.x)
				{
					return bomberman::RIGHT;
				}
			} else if (playerGridPos.x  == enemyGridPos.x)
			{

				if (playerGridPos.y > enemyGridPos.y)
				{
					return bomberman::UP;
				}

				if (playerGridPos.y < enemyGridPos.y)
				{
					return bomberman::DOWN;
				}
			}
			

		}

	}



	return -1;
}



void bomberman::Enemy::Initialize()
{
	AnimatedEntity::Initialize();

	Game::GetInstance()->AddPhysicsBody(&body);

	canDie = false;
	invicibleTimer.reset();
}

bool bomberman::Enemy::IsOnGrid(float margin)
{
	// 32 = tile size
	// 16 = tile size / 2
	// *-----*
	// |  _	 |
	// |     |
	// *-----*
	// round(axis / 32) * 32 + 16
	int2 tile1 = int2{ static_cast<int>(floor((GetPosition().x + margin) / 32)),
					static_cast<int>(floor((GetPosition().y + margin) / 32)) };
	int2 tile2 = int2{ static_cast<int>(floor((GetPosition().x - margin) / 32)),
						static_cast<int>(floor((GetPosition().y - margin) / 32)) };

	if (tile1.x != tile2.x && (direction == 0 || direction == 2))
	{
		position.x = static_cast<float>(tile1.x * 32);
	}	
	if (tile1.y != tile2.y && (direction == 1 || direction == 3))
	{

		position.y = static_cast<float>(tile1.y * 32);
	}

	return false;
}

void bomberman::Enemy::KillYourselfNow()
{
	if (isDead || !canDie) return;

	isDead = true;
	Events::CallEvent(ENEMY_KILLED, {});
}

void bomberman::Enemy::Tick(float deltaTime)
{

	Game::GetInstance()->AddEntityToRenderQueue(this);
	if (isDead) return;

	if (invicibleTimer.elapsed() > graceTime)
	{
		canDie = true;

	}

	AnimatedEntity::Tick(deltaTime);
	body.SetAABB(float4{ position.x - scale.x * (0.85f - 0.2f), position.y - scale.y * 0.85f, position.x + scale.x * (0.85f - 0.2f), position.y + scale.y * 0.85f });


	// Check the collisions
	int2 snap{ 0, 0 };
	TileType type;

	for (int i=0; i<4; i++)
	{
		dirs[i] = true;
	}

	//left
	snap = { (int)floor((body.GetAABB().x + 8) / 32) + 1 ,(int)floor((body.GetAABB().y + 5) / 32) };

	type = LevelManager::GetTile(snap).GetType();
	if (!CanWalkThroughTile(type))
	{
		dirs[LEFT] = false;
	}

	//right
	snap = { (int)floor((body.GetAABB().x - 31) / 32) + 1 ,(int)floor((body.GetAABB().y + 5) / 32) };

	type = LevelManager::GetTile(snap).GetType();
	if (!CanWalkThroughTile(type))
	{
		dirs[RIGHT] = false;
	}

	//up
	snap = { (int)floor((body.GetAABB().x - 5) / 32) + 1 ,(int)floor((body.GetAABB().y - 10) / 32) };

	type = LevelManager::GetTile(snap).GetType();
	if (!CanWalkThroughTile(type))
	{
		dirs[DOWN] = false;
	}

	//down
	snap = { (int)floor((body.GetAABB().x - 5) / 32) + 1 ,(int)floor((body.GetAABB().y + 20) / 32) };

	type = LevelManager::GetTile(snap).GetType();
	if (!CanWalkThroughTile(type))
	{
		dirs[UP] = false;
	}


}


///--------- Basic enemy
void bomberman::BasicEnemy::PrepareRender()
{
	if (timer.elapsed() > 0.2)
	{
		timer.reset();

		currentFrame.x++;

		if (isDead) {
			if (currentFrame.x < 6)
			{
				currentFrame.y = 15;
				currentFrame.x = 7;
			}
			if (currentFrame.x == 10)
			{
				position = { 0, -10 };
				scale = { 0, 0 };
				Game::GetInstance()->AddToScore(GetRewardAmount());
				body.SetAABB({ 0, 0, 0, 0 });
			}
		}
		else {

			if ((direction == 3 || direction == 0) && currentFrame.x > 2) {
				currentFrame.x = 0;
			}  if ((direction == 1 || direction == 2) && (currentFrame.x > 5 || currentFrame.x < 3)) {
				currentFrame.x = 3;
			}
		}



	}


	Enemy::PrepareRender();
	shader->SetFloat("layer", 1.0);
}

void bomberman::BasicEnemy::Initialize()
{
	Enemy::Initialize();

	scale = { 16, 16 };
	currentFrame = GetStarterFrame();

	body.SetPosition(position);
}

void bomberman::BasicEnemy::Tick(float deltaTime)
{
	Enemy::Tick(deltaTime);

	if (isDead) return;


	if (!GetAllowedDirections()[direction])
	{
		direction = Vec2ToDir(-DirToVec2[direction]);

		int rD = rand() % 4;

		if (GetAllowedDirections()[rD])
		{
			direction = rD;
		}

	}




	body.AddForce((DirToVec2[direction] * GetSpeed()) * deltaTime);
	position = body.GetPosition();

	//snap the position to the grid
	int2 snap = int2{ (int)floor((body.GetAABB().x) / 32) ,(int)floor((body.GetAABB().y) / 32) };

	if (direction == 0 || direction == 2)
	{
		position.x = (snap.x + 1.0f) * 32;
	}
	body.SetPosition(position);


}

void bomberman::BasicEnemy::Destroy()
{
	Enemy::Destroy();
}

///------ Advanced enemy

void bomberman::AdvancedEnemy::Tick(float deltaTime)
{
	Enemy::Tick(deltaTime);

	if (isDead) return;

	for (Player* player : Game::GetInstance()->GetPlayers())
	{
		if (player->GetState() != DYING)
		{
			int dir = GetDirectionToPlayer(*this, *player);
			if (dir >= 0 && GetAllowedDirections()[direction]) direction = dir;
		}

	}


	if (!GetAllowedDirections()[direction])
	{
		direction = Vec2ToDir(-DirToVec2[direction]);

		int rD = rand() % 4;
		while (!GetAllowedDirections()[rD])
		{
			rD = rand() % 4;
		}
		direction = rD;

	}


	IsOnGrid(32.0f);



	body.AddForce((DirToVec2[direction] * GetSpeed()) * deltaTime);
	position = body.GetPosition();

	//snap the position to the grid
	int2 snap = int2{ (int)floor((body.GetAABB().x) / 32) ,(int)floor((body.GetAABB().y) / 32) };

	if (direction == 0 || direction == 2)
	{
		position.x = (snap.x + 1.0f) * 32.0f;
	}
	body.SetPosition(position);
}

void bomberman::Ovapi::Initialize()
{
	BasicEnemy::Initialize();

	SetCanWalkThroughTile(BREAKABLE, true);
	SetCanWalkThroughTile(BOMB, true);
}

void bomberman::Kondoria::Initialize()
{
	AdvancedEnemy::Initialize();

	SetCanWalkThroughTile(BREAKABLE, true);
	SetCanWalkThroughTile(BOMB, true);
}

void bomberman::Pontan::Initialize()
{
	BasicEnemy::Initialize();

	SetCanWalkThroughTile(BREAKABLE, true);
	SetCanWalkThroughTile(BOMB, true);
}

void bomberman::DebugEnemy::Initialize()
{
	AdvancedEnemy::Initialize();
	SetCanWalkThroughTile(BREAKABLE, true);
}

