#include "precomp.h"
#include "bomb.h"

#include "../core/audio.h"
#include "../../game.h"
#include "../core/levelManager.h"


void bomberman::Explosion::Initialize()
{
	AnimatedEntity::Initialize();

	currentFrame = explosionKeyFrames[type][direction];
	scale = {16,16};
}

void bomberman::Explosion::Tick(float deltaTime)
{
	AnimatedEntity::Tick(deltaTime);

	if (state > PASSIVE)
	{
		Bomb* bomb = (Bomb*)parent;
		bomb->Clear();
	}

	if (timer.elapsed() > 0.1)
	{
		animationState++;
		if (animationState > 3)
		{
			animationState = 0;
			state++;
		}

		currentFrame = explosionKeyFrames[type][direction];
		currentFrame.x += 5 * animationState;

		timer.reset();
	}


	Game::GetInstance()->AddEntityToRenderQueue(this);
}

bomberman::Bomb::~Bomb()
{

	Game::GetInstance()->RemovePhysicsBody(explosionTriggerNorth.GetID());
	Game::GetInstance()->RemovePhysicsBody(explosionTriggerSouth.GetID());
	Game::GetInstance()->RemovePhysicsBody(explosionTriggerWest.GetID());
	Game::GetInstance()->RemovePhysicsBody(explosionTriggerEast.GetID());

	for (Explosion* explosion : explosions)
	{
		delete explosion;
		explosion = nullptr;
	}

	explosions.clear();
}

void bomberman::Bomb::PrepareRender()
{
	AnimatedEntity::PrepareRender();
	shader->SetFloat("layer", 1.0);
}

void bomberman::Bomb::Initialize()
{
	AnimatedEntity::Initialize();

	scale = {16, 16};
	currentFrame = {0, 3};
	filterColor = {0, 1, 0, 1};
	position = NormalSpaceToGridSpace({ position.x, position.y}) * int2 { 32, 32};
	
	explosionTriggerNorth = PhysicsObject{ float4{position.x - scale.x * 1.0f - 0.1f, position.y - scale.y * 1.0f,
												position.x + scale.x * 1.0f - 0.1f, position.y + scale.y * 1.0f},
		ObjectType::Static };

	explosionTriggerWest = PhysicsObject{ float4{position.x - scale.x * 1.0f - 0.1f, position.y - scale.y * 1.0f,
											position.x + scale.x * 1.0f - 0.1f, position.y + scale.y * 1.0f},
	ObjectType::Static };

	explosionTriggerSouth = PhysicsObject{ float4{position.x - scale.x * 1.0f - 0.1f, position.y - scale.y * 1.0f,
											position.x + scale.x * 1.0f - 0.1f, position.y + scale.y * 1.0f},
	ObjectType::Static };

	explosionTriggerEast = PhysicsObject{ float4{position.x - scale.x * 1.0f - 0.1f, position.y - scale.y * 1.0f,
											position.x + scale.x * 1.0f - 0.1f, position.y + scale.y * 1.0f},
	ObjectType::Static };

	Game::GetInstance()->AddPhysicsBody(&explosionTriggerNorth);
	Game::GetInstance()->AddPhysicsBody(&explosionTriggerEast);
	Game::GetInstance()->AddPhysicsBody(&explosionTriggerSouth);
	Game::GetInstance()->AddPhysicsBody(&explosionTriggerWest);
}

void bomberman::Bomb::Tick(float deltaTime)
{
	AnimatedEntity::Tick(deltaTime);
	
	
	if (timer.elapsed() > 0.2 && state == ACTIVATED)
	{
		timer.reset();
		currentFrame.x += 1;
		if (currentFrame.x > 2)
		{
			stage++;
			currentFrame.x = 0;
		}
	}

	if (stage > 3 && state == ACTIVATED)
	{
		Audio::PlaySound(SoundType::EXPLOSION);
		Explode();
	}
	if (state == EXPLODING)
	{
		explosionTriggerNorth.SetAABB(float4{ position.x - collisionSize, position.y - (collisionSize + (explosionColSizes[NORTH] * 32)),
				position.x + collisionSize, position.y + collisionSize });
		explosionTriggerSouth.SetAABB(float4{ position.x - collisionSize, position.y + collisionSize,
				position.x + collisionSize, position.y + (collisionSize + (explosionColSizes[SOUTH] * 32)) });
		explosionTriggerEast.SetAABB(float4{ position.x - collisionSize, position.y - collisionSize,
		position.x + (collisionSize + (explosionColSizes[EAST] * 32)), position.y + collisionSize });
		explosionTriggerWest.SetAABB(float4{ position.x - (collisionSize + (explosionColSizes[WEST] * 32)), position.y - collisionSize,
position.x + collisionSize, position.y + collisionSize });

		Game::GetInstance()->IsKillerColliding(explosionTriggerWest.GetAABB(), KillerType::FIRE, true);
		Game::GetInstance()->IsKillerColliding(explosionTriggerNorth.GetAABB(), KillerType::FIRE, true);
		Game::GetInstance()->IsKillerColliding(explosionTriggerEast.GetAABB(), KillerType::FIRE, true);
		Game::GetInstance()->IsKillerColliding(explosionTriggerSouth.GetAABB(), KillerType::FIRE, true);

		LevelManager::IsCollidingWithAnyEnemy(*this, explosionTriggerNorth, true);
		LevelManager::IsCollidingWithAnyEnemy(*this, explosionTriggerEast, true);
		LevelManager::IsCollidingWithAnyEnemy(*this, explosionTriggerSouth, true);
		LevelManager::IsCollidingWithAnyEnemy(*this, explosionTriggerWest, true);

		for (Explosion* explosion : explosions)
		{
			explosion->Tick(deltaTime);
		}
	}



	Game::GetInstance()->AddEntityToRenderQueue(this);
}

void bomberman::Bomb::Explode()
{
	float2 nScale = { 32, 32 };
	if (state == ACTIVATED)
	{
		for (int i = 0; i < 4; i++) explosionColSizes[i] = 0.0f;

		const int2 dirs[4] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}}; 
		const float colliderSize = 0.98f;
		bool allowedDirs[4] = {true, true, true, true };
		int2 index = { (int)position.x / 32,(int)position.y / 32};
		

		printf("index %i, %i\n", index.x, index.y);
		state = EXPLODING;



		// 0 = center

		for (int k = 0; k < 4; k++)
		{
			for (int i = 1; i < Bomb::explosionRange; i++)
				if (allowedDirs[k]) {
					ExplosionType type = MIDDLE;
					if (i >= Bomb::explosionRange - 1)
					{
						explosionColSizes[k] = i * colliderSize;
						type = END;
					}
					const int2 tileIndex = index + (dirs[k] * (i));
					Tile& tile = LevelManager::GetTile(tileIndex);
					if (tile.GetType() == EMPTY)
					{
						Explosion* explosion = new Explosion{};
						int2 offset = dirs[k];

						explosion->SetType(type);

						explosion->SetDirection(static_cast<ExplosionDirection>(k));

						explosion->Initialize();


						explosion->SetPosition(
							this->GetPosition() + (nScale * (offset * i))
						);

						explosion->SetParent(this);

						explosions.push_back(explosion);
					} else
					{

						explosionColSizes[k] = i * colliderSize;
						tile.Explode();
						allowedDirs[k] = false;
					}
				}
			

		}

		


		Explosion* nExplosion = new Explosion{};

		nExplosion->SetType(CENTER);

		nExplosion->Initialize();
		nExplosion->SetPosition(this->position);


		nExplosion->SetParent(this);

		printf("range: %i\n", explosionRange);
		explosions.push_back(nExplosion);



		scale = {0, 0};



	}
}

void bomberman::Bomb::Activate()
{
	currentFrame = {1, 3};
	state = ACTIVATED;
}

void bomberman::Bomb::Clear()
{
	state = EXPLODED;

} 
