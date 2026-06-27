#include "precomp.h"
#include "player.h"


#include "../core/assetManager.h"
#include "../core/audio.h"
#include "game.h"
#include "../core/levelManager.h"

namespace
{
	static float colliderSize = 0.85f;
	uint idCounter = 0;
}

void bomberman::Player::Initialize()
{
	AnimatedEntity::Initialize();
	if (idCounter >= MAX_PLAYERS) idCounter = 0;
	playerId = idCounter++;
	printf("New player: %i connected!\n", playerId);
	position = playerSpawnPositions[playerId];
	scale = { 16, 16 };
	state = MOVING;

	filterColor = { 0, 1, 0, 1.0 };
	frameSize = {16, 384 };
	currentFrame = { 0, 0};

	secondaryColor = {1, 1, 1, 1};
	replaceColor = playerColors[playerId];

	body = PhysicsObject{float4{position.x - scale.x * colliderSize - 0.1f, position.y - scale.y * colliderSize, position.x + scale.x * colliderSize - 0.1f, position.y + scale.y * colliderSize}, ObjectType::Rigid };
	for (int i=0; i<5; i++)
	{
		placedOnTiles[i] = nullptr;
	}

}

void bomberman::Player::PrepareRender()
{
	AnimatedEntity::PrepareRender();

	if (animationTimer.elapsed() > frameDelay)
	{

		animationTimer.reset();
		if (state == MOVING)
		{

			currentFrame.y = keyFrames[dir].x;
			currentFrame.x++;

			// if the currentframe is smaller than the start frame
			if (currentFrame.x < keyFrames[dir].y)
			{
				Audio::PlaySound(dir == LEFT || dir == RIGHT ? SoundType::WALKING_LR : SoundType::WALKING_UD, 0.5f);
				currentFrame.x = keyFrames[dir].y;
			}
			if (currentFrame.x > keyFrames[dir].z)
			{
				Audio::PlaySound(dir == LEFT || dir == RIGHT ? SoundType::WALKING_LR : SoundType::WALKING_UD  , 0.5f);
				currentFrame.x = keyFrames[dir].y;
			}
		}
		if (state == DYING)
		{
			currentFrame.y = keyFrames[4].x;
			currentFrame.x++;

			if (currentFrame.x < keyFrames[4].y)
			{
				currentFrame.x = keyFrames[4].y;
			}
			if (currentFrame.x > keyFrames[4].z)
			{
				scale = {0, 0};
				
			}
		}

	}

}

void bomberman::Player::Tick(float deltaTime)
{
	if (state == DYING) {
		frameDelay = keyFrames[4].w * 0.1f;
		return;
	}

	position = body.GetPosition();

	body.SetAABB(float4{ position.x - scale.x * (colliderSize - 0.2f), position.y - scale.y * colliderSize, position.x + scale.x * (colliderSize - 0.2f), position.y + scale.y * colliderSize });
	state = STANDING;
	frameDelay = keyFrames[dir].w * 0.1f;

	CollisionCheck();



	//Check how many of our bombs are alive;
	placedBombs = 0;
	for (int i=0; i<maxBombs; i++)
	{
		const Tile* tile = placedOnTiles[i];
		if (tile != nullptr)
		{
			if (!tile->HasBomb())
			{
				placedOnTiles[i] = nullptr;
			} else
			{
				placedBombs++;
			}
		} 
	}
	

	if (GetAsyncKeyState(controllers[playerId][4]) && placedBombs < maxBombs)
	{
		Tile& tile = LevelManager::GetTile({ (int)floor(position.x / 32) ,(int)floor(position.y / 32) });

		if (!tile.HasBomb()) {
			tile.SpawnBomb(NormalSpaceToGridSpace({ position.x + 16, position.y }), explodeOnSpawn);
			Audio::PlaySound(SoundType::PLACE_BOMB);
			placedOnTiles[placedBombs] = &tile;

		}
	}

	if (GetAsyncKeyState(controllers[playerId][UP]) && !collisions[UP])
	{
		state = MOVING;
		dir = UP;
		body.AddForce({ 0, movementSpeed * deltaTime });
	}
	  if (GetAsyncKeyState(controllers[playerId][DOWN]) && !collisions[DOWN])
	{
		state = MOVING;
		dir = DOWN;
		body.AddForce({ 0, -movementSpeed * deltaTime });
	}
	  if (GetAsyncKeyState(controllers[playerId][RIGHT]) && !collisions[RIGHT])
	{
		state = MOVING;
		dir = RIGHT;
		body.AddForce({ -movementSpeed * deltaTime, 0 });
	}
	if (GetAsyncKeyState(controllers[playerId][LEFT]) && !collisions[LEFT])
	{
		state = MOVING;
		dir = LEFT;
		body.AddForce({ movementSpeed * deltaTime, 0 });
	}
}

void bomberman::Player::Destroy()
{
}

#ifdef GOD_MODE
void bomberman::Player::KillYourself(const KillerType& )
#else
void bomberman::Player::KillYourself(const KillerType& killerType)
#endif
{
#ifdef GOD_MODE
	return;
#else
	if (state == DYING || immuneTo[killerType]) return;

	state = DYING;
	currentFrame.y = 2;
	currentFrame.x = 0;
	idCounter = 0;
#endif




}

//TODO: Maybe do a better way to do this? All the repetition doesnt  look good.
void bomberman::Player::CollisionCheck()
{
	// So we check 2 points for each direction.
	// this is to check if we are clipping a tile.
	//if so we disable movement for that direction.
	int2 snapA{ 0, 0 };
	int2 snapB{ 0, 0 };

	Tile* tileA = nullptr; 
	Tile* tileB = nullptr;

	for (int i=0; i<4; i++)
	{
		collisions[i] = false;
 	}
	//right

	{
		int2 snap = int2{ (int)floor((body.GetAABB().x - scale.x / 3) / 32) + 1 ,(int)floor((body.GetAABB().y) / 32) };
		//bool isStandingOnBomb = LevelManager::GetTile(snap).HasBomb();

		snapA = { (int)floor((body.GetAABB().x + 5) / 32) + 1 ,(int)floor((body.GetAABB().y + 15) / 32) };
		snapB = { (int)floor((body.GetAABB().x + 5) / 32) + 1 ,(int)floor((body.GetAABB().y - 5) / 32) };

		tileA = &LevelManager::GetTile(snapA);
		tileB = &LevelManager::GetTile(snapB);

		if (!CanWalkThroughTile(tileA->GetType()) || !CanWalkThroughTile(tileB->GetType()))
		{
			collisions[RIGHT] = true;
		}

		if (tileA->GetType() == BOMB || tileB->GetType() == BOMB)
		{
			snapA.x = (int)floor((body.GetAABB().x + 10) / 32) + 1;
			snapB.x = (int)floor((body.GetAABB().x + 10) / 32) + 1;


			tileA = &LevelManager::GetTile(snapA);
			tileB = &LevelManager::GetTile(snapB);

			if (CanWalkThroughTile(tileA->GetType()) || CanWalkThroughTile(tileB->GetType()))
			{
				collisions[RIGHT] = false;
			}
		}
	}
	//left
	{
		snapA = { (int)floor((body.GetAABB().x - 25) / 32) + 1 ,(int)floor((body.GetAABB().y + 15) / 32) };
		snapB = { (int)floor((body.GetAABB().x - 25) / 32) + 1 ,(int)floor((body.GetAABB().y - 5) / 32) };

		tileA = &LevelManager::GetTile(snapA);
		tileB = &LevelManager::GetTile(snapB);

		if (!CanWalkThroughTile(tileA->GetType()) || !CanWalkThroughTile(tileB->GetType()))
		{
			collisions[LEFT] = true;
		}

		// if we are standing in a bomb we check further on so that we can still walk out of a bomb if we are in one.
		if (tileA->GetType() == BOMB || tileB->GetType() == BOMB)
		{
			snapA.x = (int)floor((body.GetAABB().x - 30) / 32) + 1;
			snapB.x = (int)floor((body.GetAABB().x - 30) / 32) + 1;


			tileA = &LevelManager::GetTile(snapA);
			tileB = &LevelManager::GetTile(snapB);

			if (CanWalkThroughTile(tileA->GetType()) || CanWalkThroughTile(tileB->GetType()))
			{
				collisions[LEFT] = false;
			}
		}
	}
	//up
	{
		snapA = { (int)floor((body.GetAABB().x) / 32) + 1 , (int)floor((body.GetAABB().y - 10) / 32) };
		snapB = { (int)floor((body.GetAABB().x - 10) / 32) + 1 , (int)floor((body.GetAABB().y - 10) / 32) };

		tileA = &LevelManager::GetTile(snapA);
		tileB = &LevelManager::GetTile(snapB);


		if (!CanWalkThroughTile(tileA->GetType()) || !CanWalkThroughTile(tileB->GetType()))
		{
			collisions[UP] = true;
		}

		// if we are standing in a bomb we check further on so that we can still walk out of a bomb if we are in one.
		if (tileA->GetType() == BOMB || tileB->GetType() == BOMB)
		{
			snapA.y = (int)floor((body.GetAABB().y - 15) / 32);
			snapB.y = (int)floor((body.GetAABB().y - 15) / 32);


			tileA = &LevelManager::GetTile(snapA);
			tileB = &LevelManager::GetTile(snapB);

			if (CanWalkThroughTile(tileA->GetType()) || CanWalkThroughTile(tileB->GetType()))
			{
				collisions[UP] = false;
			}
		}
	}
	//down
	{
		snapA = { (int)floor((body.GetAABB().x) / 32) + 1 , (int)floor((body.GetAABB().y + 20) / 32) };
		snapB = { (int)floor((body.GetAABB().x - 10) / 32) + 1 , (int)floor((body.GetAABB().y + 20) / 32) };

		tileA = &LevelManager::GetTile(snapA);
		tileB = &LevelManager::GetTile(snapB);

		if (!CanWalkThroughTile(tileA->GetType()) || !CanWalkThroughTile(tileB->GetType()))
		{
			collisions[DOWN] = true;
		}

		if (tileA->GetType() == BOMB || tileB->GetType() == BOMB)
		{
			snapA.y = (int) floor((body.GetAABB().y + 25) / 32);
			snapB.y = (int)floor((body.GetAABB().y + 25) / 32);


			tileA = &LevelManager::GetTile(snapA);
			tileB = &LevelManager::GetTile(snapB);

			if (CanWalkThroughTile(tileA->GetType()) || CanWalkThroughTile(tileB->GetType()))
			{
				collisions[DOWN] = false;
			}
		}
	}
}
