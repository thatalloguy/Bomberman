#pragma once
#include "../core/entity.h"
#include <GLFW/glfw3.h>

#include "../core/physics.h"

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 2
#endif

//#define GOD_MODE

namespace bomberman
{
	enum KillerType: int;
}

namespace bomberman
{
	static uint controllers[MAX_PLAYERS][6] = {
		{ // down, right, up, left, A action. B action
			GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_E, GLFW_KEY_Q
		},
		{ 
			VK_DOWN, VK_RIGHT, VK_UP, VK_LEFT, VK_RSHIFT, GLFW_KEY_SLASH
		},
		/*{
			GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_Y, GLFW_KEY_G, GLFW_KEY_U
		}*/
	};

	static float4 playerColors[MAX_PLAYERS]
	{
		{1.0, 0.8, 0.8, 1},
		{0.8, 1.0, 0.8, 1}
	};

	static float2 playerSpawnPositions[MAX_PLAYERS]
	{
		{16 * 4, 16 * 4.5f},
		{32 * 25, 32 * 12 }
	};

	enum MovementDirection: int
	{
		LEFT = 3,
		DOWN = 0,
		RIGHT = 1,
		UP = 2,
	};

	static float4 keyFrames[]
	{ // the y index,   the first and last frame , the frame duration times ten
		{0.0f, 3.0f, 5.0f, 1.0f}, // down
		{1.0f, 0.0f, 2.0f, 1.0f}, // right
		{1.0f, 3.0f, 5.0f, 1.0f}, // up
		{0.0f, 0.0f, 2.0f, 1.0f}, // left
		{2.0f, 0.0f, 6.0f, 2.0f} // die!
	};

	enum PlayerState: uint
	{
		MOVING = 0,
		STANDING = 1,
		DYING = 2,
	};


	class Player : public AnimatedEntity
	{
	public:
		~Player() override = default;

		int GetPlayerId() const { return playerId; }

		void Initialize() override;
		void PrepareRender() override;
		void Tick(float deltaTime) override;
		void Destroy() override;

		void KillYourself(const KillerType& killerType);

		float GetMovementSpeed() const { return movementSpeed;  }
		void SetMovementSpeed(float f) { movementSpeed = f; };

		int  GetMaxBombs() const { return maxBombs; };
		void SetMaxBombs(int i) { maxBombs = i; };

		bool CanWalkThroughTile(const TileType type) const { return walkableTiles[type]; }
		void SetCanWalkThroughTile(const TileType type, bool b) { walkableTiles[type] = b; };

		bool IsImmuneTo(const KillerType type) const { return immuneTo[type]; };
		void SetImmuneTo(const KillerType type, bool b) { immuneTo[type] = b; };

		void SetDirection(MovementDirection dire) { dir = dire; }
		Tile** GetPlacedOnTiles() { return placedOnTiles; };
		void SetBombExplodeOnSpawn(bool b) { explodeOnSpawn = b; };

		PlayerState			GetState() const { return state; }
		MovementDirection	GetDirection() const { return dir;  }
		uint				GetPlayerID() const { return playerId;  }
		PhysicsObject*		GetRigidbody() { return &body; };


	protected:
		void CollisionCheck();

		float movementSpeed = 2.2f;
		PlayerState state = STANDING;
		MovementDirection dir = UP;

	private:

		bool immuneTo[3] = 
		{
			// fire
			false,
			// enemies,
			false,
			// Misc
			true,
		};

		bool walkableTiles[8] = {
			// unknown
			false,
			// empty
			true,
			// solid
			false,
			// breakable
			false,
			// exit
			true,
			// powerup
			true,
			//bomb: note this one changes with the bombwalk powerup
			false,
			//Bonus
			true
		};
		bool collisions[4] = {false, false, false, false};
		bool dead = false;

		int maxBombs = 1;
		int placedBombs = 0;
		bool explodeOnSpawn = true; // default : true

		float frameDelay = 0.1f;

		Tile* placedOnTiles[5] = {nullptr, nullptr, nullptr, nullptr};

		Timer animationTimer;
		PhysicsObject body;
		uint playerId = 0;

	};
}
