#pragma once
#include "../core/entity.h"
#include "../core/physics.h"

namespace bomberman
{

	enum class EnemyType: int
	{
		UNKNOWN = 0,
		VALCOM = 1,
		ONEAL = 2,
		DALL = 3,
		MINVO = 4,
		OVAPI = 5,
		KONDORIA = 6,
		PARSE = 7,
		PONTAN = 8
	};



	static float2 DirToVec2[4] =
	{
		float2{ 0, 1 }, // down
		float2{ 1, 0 }, // Right
		float2{ 0, -1 }, // up
		float2{ -1, 0 }, // left
	};



	inline int Vec2ToDir(const float2& vec)
	{
		if (vec.x == 0 && vec.y == 1)
		{
			return 0;
		}

		if (vec.x == 1 && vec.y == 0)
		{
			return 1;
		}

		if (vec.x == 0 && vec.y == -1)
		{
			return 2;
		}

		if (vec.x == -1 && vec.y == 0)
		{
			return 3;
		}

		return 1;
	}

	class Enemy: public AnimatedEntity
	{
	public:
		~Enemy() override = default;

		void Initialize() override;
		void Tick(float deltaTime) override;

		bool IsOnGrid(float margin);

		const PhysicsObject& GetBody() const { return body;  };

		void KillYourselfNow();
		bool IsDead() const { return isDead;  }

		bool CanWalkThroughTile(const TileType type) const { return walkableTiles[type]; }

	protected:
		void SetCanWalkThroughTile(const TileType type, bool b) { walkableTiles[type] = b; };
		bool* GetAllowedDirections() { return dirs; };

		bool isDead = false;
		bool canDie = true;
		const float graceTime = 1.0f;
		Timer invicibleTimer{};

		PhysicsObject body{};
		Timer timer{};

		int direction = 1;

	private:

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

		bool dirs[4]{ true, true, true, true };
	};

	class BasicEnemy: public Enemy
	{
	public:
		~BasicEnemy() override = default;

		void PrepareRender() override;
		void Initialize() override;
		void Tick(float deltaTime) override;
		void Destroy() override;


		virtual int GetRewardAmount() const { return 100; };
		virtual float GetSpeed() const { return 2.0f; };
		virtual int2 GetStarterFrame() const { return { 0, 15 }; }

	};

	class AdvancedEnemy: public BasicEnemy
	{
	public:
		~AdvancedEnemy() override = default;

		void Tick(float deltaTime) override;
	};

	class Valcom: public BasicEnemy
	{
	public:
		~Valcom() override = default;
	};

	// Oneal is the same as valcom except it does use the pathfinding.
	class Oneal : public AdvancedEnemy
	{
	public:
		~Oneal() override = default;

		int GetRewardAmount() const override { return 200; };
		float GetSpeed() const override { return 2.2f; };
		int2 GetStarterFrame() const override { return { 0, 16 }; }
	};

	class Dall: public AdvancedEnemy
	{
	public:
		~Dall() override = default;

		int GetRewardAmount() const override { return 400; };
		float GetSpeed() const override { return 2.5f; };
		int2 GetStarterFrame() const override { return { 0, 17 }; };
	};

	class Minvo: public AdvancedEnemy
	{
	public:
		~Minvo() override = default;

		int GetRewardAmount() const override { return 800; };
		float GetSpeed() const override { return 2.75f; };
		int2 GetStarterFrame() const override { return { 0, 18 }; };
	};

	class Ovapi : public BasicEnemy
	{
	public:
		~Ovapi() override = default;

		void Initialize() override;

		int GetRewardAmount() const override { return 1000; };
		float GetSpeed() const override { return 2.0f; };
		int2 GetStarterFrame() const override { return { 0, 20 }; };
	};

	class Kondoria: public AdvancedEnemy
	{
	public:
		~Kondoria() override = default;

		void Initialize() override;


		int GetRewardAmount() const override { return 2000; };
		float GetSpeed() const override { return 0.75f; };
		int2 GetStarterFrame() const override { return { 0, 19 }; };
	};

	class Parse: public AdvancedEnemy
	{
	public:
		~Parse() override = default;

		int GetRewardAmount() const override { return 4000; };
		float GetSpeed() const override { return 3.0f; };
		int2 GetStarterFrame() const override { return { 0, 21 }; };
		
	};

	class Pontan : public BasicEnemy
	{
	public:
		~Pontan()override = default;

		void Initialize() override;


		int GetRewardAmount() const override { return 8000; };
		float GetSpeed() const override { return 3.25f; };
		int2 GetStarterFrame() const override { return { 0, 22 }; };
	};

	class DebugEnemy : public AdvancedEnemy
	{
	public:
		~DebugEnemy()override = default;

		void Initialize() override;


		int GetRewardAmount() const override { return 8000; };
		float GetSpeed() const override { return 5.25f; };
		int2 GetStarterFrame() const override { return { 0, 23 }; };
	};
}
